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



#include "ofxNeetsSwitchingRelay.h"


//------------------------------------------------------------------
ofxNeetsSwitchingRelay::ofxNeetsSwitchingRelay(){


};


//------------------------------------------------------------------
ofxNeetsSwitchingRelay::~ofxNeetsSwitchingRelay(){
    ofRemoveListener(ofEvents().update, this, &ofxNeetsSwitchingRelay::update);

};



//------------------------------------------------------------------
void ofxNeetsSwitchingRelay::setup(string ip, int port, int uid) {
	
    ofLogNotice("ofxNeetsSwitchingRelay")<< "Connecting to host \"" << ip << "\" port "
	<< port << "\" uid: \"" << uid << "\"";

    hostIP = ip;
    controlPort = port;
    unitId = uid;
    
    client.setup(hostIP, controlPort, false);
	client.setMessageDelimiter("\r\n");

    ofAddListener(ofEvents().update, this, &ofxNeetsSwitchingRelay::update);
}


void ofxNeetsSwitchingRelay::update(ofEventArgs &e){

    if(!client.isConnected() && ofGetElapsedTimef() - lastReconnectTry > reconnectWait && cmds.size()){
		ofLogWarning("ofxNeetsSwitchingRelay") << "reconnecting!";
        client.setup(hostIP,controlPort,false);
        lastReconnectTry = ofGetElapsedTimef();
    }
    
    if(client.isConnected() && cmds.size()){
        client.send(cmds[0]);
        cmds.erase(cmds.begin());
        if(cmds.size() == 0){
            close();
        }
    }
    
};

//------------------------------------------------------------------
void ofxNeetsSwitchingRelay::turnOnSocket(int socketId, float time, float delay) {
    if(socketId<1 || socketId>4){
        ofLogError("ofxNeetsSwitchingRelay") << "ofxNeetsSwitchingRelay relay range is 1 - 4";
        return;
    }
	ofLogNotice("ofxNeetsSwitchingRelay")<< "turnOnSocket " << socketId;
    sendAction("SET",socketId,time,delay);
}
void ofxNeetsSwitchingRelay::turnOffSocket(int socketId, float time, float delay) {
    if(socketId<1 || socketId>4){
        ofLogError("ofxNeetsSwitchingRelay") << "ofxNeetsSwitchingRelay relay range is 1 - 4";
        return;
    }
	ofLogNotice("ofxNeetsSwitchingRelay")<< "turnOffSocket " << socketId;
    sendAction("RELEASE",socketId,time,delay);
}

void ofxNeetsSwitchingRelay::sendAction(string action, int socketId, float time, float delay){
    
    string cmd = "NEUNIT="+ofToString(unitId)+",RELAY="+ofToString(socketId)+",ACTION="+action+",TIME="+ofToString(time)+"\\CR";
    
    if(delay>0){
        cmd = "NEUNIT="+ofToString(unitId)+",RELAY="+ofToString(socketId)+",ACTION="+action+",DELAY="+ofToString(time)+"\\CR";
    }
    
    //ofLogNotice("ofxNeetsSwitchingRelay")<< "Sending Action: " << cmd;
    
    if(client.isConnected()){
        client.send(cmd);
    }else{
        cmds.push_back(cmd);
    }
};


void ofxNeetsSwitchingRelay::close(){
    client.close();
	ofLogNotice("ofxNeetsSwitchingRelay")<< "Closed Connection";
};
