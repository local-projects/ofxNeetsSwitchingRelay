// =============================================================================
//
// ofxNeetsSwitchingRelay.cpp
// NeetsSwitchingRelay
//
// Created by Andreas Borg on 9/21/16
//
// Copyright (c) 2015-2016 Andreas Borg <http://crea.tion.to>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// =============================================================================

/*
 
 FROM OSX COMMANDLINE
 
 //set NEW LAN CONFIG
 echo "NEUNIT=1,SETTINGS=LAN,IPADDRESS=192.168.102.16,SUBNET=255.255.255.0,GATEWAY=192.168.102.1\\CR\r\n" | ncat 192.168.254.252 5000  -v

 //save cfg - make sure it replies "OK" otherwise when you cut power it will go back to its default IP
 echo "NEUNIT=1,SAVE=TRUE\\CR\r\n" | ncat 192.168.102.16 5000 -d 1 | say
 
 //activate relay port 1
 echo "NEUNIT=1,RELAY=1,ACTION=RELEASE,TIME=0\\CR\r\n" | ncat 192.168.254.252 5000  -v
 
 */

#include "ofxNeetsSwitchingRelay.h"


//------------------------------------------------------------------
ofxNeetsSwitchingRelay::~ofxNeetsSwitchingRelay(){

	waitForThread(true, 5000);
};



//------------------------------------------------------------------
bool ofxNeetsSwitchingRelay::setup(string ip, int port, int uid) {

	if (isSetup) {
		ofLogError("ofxNeetsSwitchingRelay") << "already setup! dont setup twice!";
	} else {
		ofLogNotice("ofxNeetsSwitchingRelay") << "Connecting to host \"" << ip << "\" port "
			<< port << "\" uid: \"" << uid << "\"";

		settings.address = ip;
		settings.port = port;
		settings.blocking = true;
		settings.messageDelimiter = "\r\n";
		unitId = uid;
		
		ofxTCPClient tempClient;
		bool ok = tempClient.setup(settings);
		if(!ok){
			ofLogError("ofxNeetsSwitchingRelay") << "Can't connect to host \"" << ip << "\" port "
			<< port << "\" uid: \"" << uid << "\"";
		}
		tempClient.close();
		isSetup = true;
		return ok;
	}
	return false;
}

bool ofxNeetsSwitchingRelay::connect(ofxTCPClient & client){

	bool connected = client.setup(settings);
	int numFail = 0;
	
	while(!connected && isThreadRunning()){ //try reconnect if not connected
		numFail++;
		ofLogWarning("ofxNeetsSwitchingRelay") 	<< "cant connect to relay at \"" << settings.address << ":" << settings.port
		<< " - will keep trying... (" << numFail << ")" ;
		int n = 100;
		for(int i = 0; i < n; i++){
			sleep(1000 * reconnectWait / float(n));
			if(!isThreadRunning()) return false; //we have been asked to stop the thread, so we return early
		}
		connected = client.setup(settings);
	}
	return connected;
}


void ofxNeetsSwitchingRelay::threadedFunction(){

	int nCommands = 0;
	
	lock();
	nCommands = cmds.size();
	unlock();

	//at this point we should be connected
	while(nCommands > 0){
		
		ofxTCPClient client; //each new command starts a new TCP connection!
		bool connected = connect(client);

		if(!connected){ //this will only happen if we are trying to stop the thread
			return; //otherwise connect will keep trying forever
		}
		lock();
		string cmd = cmds[0];
		cmds.erase(cmds.begin());
		unlock();

		bool sent = client.send(cmd);
		client.close();
		
		lock();
		nCommands = cmds.size();
		unlock();
	}
}



void ofxNeetsSwitchingRelay::turnOnSocket(int socketId, bool blocking, float time, float delay) {
	if (!isSetup) {
		ofLogError("ofxNeetsSwitchingRelay") << "not setup!";
		return;
	}
    if(socketId<1 || socketId>4){
        ofLogError("ofxNeetsSwitchingRelay") << "ofxNeetsSwitchingRelay relay range is 1 - 4";
        return;
    }
	ofLogNotice("ofxNeetsSwitchingRelay")<< "turnOnSocket " << socketId << " blocking: " << blocking ;
	if(blocking){ //lets connect right
		string cmd = createAction("SET", socketId, time, delay);
		sendCmdBlocking(cmd);
	}else{
		sendAction("SET",socketId,time,delay);
	}
}


void ofxNeetsSwitchingRelay::turnOffSocket(int socketId, bool blocking, float time, float delay) {
	if (!isSetup) {
		ofLogError("ofxNeetsSwitchingRelay") << "not setup!";
		return;
	}
	if(socketId<1 || socketId>4){
        ofLogError("ofxNeetsSwitchingRelay") << "ofxNeetsSwitchingRelay relay range is 1 - 4";
        return;
    }
	ofLogNotice("ofxNeetsSwitchingRelay")<< "turnOffSocket " << socketId << " blocking: " << blocking ;
	
	if(blocking){ //lets connect right
		string cmd = createAction("RELEASE", socketId, time, delay);
		sendCmdBlocking(cmd);
	}else{
		  sendAction("RELEASE",socketId,time,delay);
	}
}


void ofxNeetsSwitchingRelay::sendAction(string action, int socketId, float time, float delay){
	if (!isSetup) {
		ofLogError("ofxNeetsSwitchingRelay") << "not setup!";
		return;
	}
	string cmd = createAction(action, socketId, time, delay);

	ofLogNotice() << cmd;

	lock();
	cmds.push_back(cmd);
	unlock();
	if(!isThreadRunning()){
		startThread();
	}
};

void ofxNeetsSwitchingRelay::sendCmdBlocking(string cmd){
	ofxTCPClient tempClient;
	bool ok = tempClient.setup(settings);
	if(ok){
		tempClient.send(cmd);
		ofSleepMillis(1);
		string rec = tempClient.receiveRaw();
		ofLogNotice() << rec;
		tempClient.close();
	}else{
		ofLogError("ofxNeetsSwitchingRelay") << "cant connect to Neets Relay at " << settings.address;
	}
}


string ofxNeetsSwitchingRelay::createAction(string action, int socketId, float time, float delay){
	string cmd;
	if(delay > 0){
		cmd = "NEUNIT="+ofToString(unitId)+",RELAY="+ofToString(socketId)+",ACTION="+action+",DELAY="+ofToString(time)+"\\CR";
	}else{
		cmd = "NEUNIT="+ofToString(unitId)+",RELAY="+ofToString(socketId)+",ACTION="+action+",TIME="+ofToString(time)+"\\CR";
	}
	return cmd;
}


