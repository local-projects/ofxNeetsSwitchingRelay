#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofBackground(22);
	//relay.setup("192.168.254.252",5000);
    relay.setup("192.168.102.16",5000);
	//relay.setup("127.0.0.1",5000);
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    ofDrawBitmapString("Use 1-4 keys to turn on/off relay", 20,40);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

	bool blocking = true;

	if(key >= '1' && key <= '4' ){
		int l = key - '1' + 1; //1..4
		if(status[key]){
			relay.turnOffSocket(l, blocking);
			status[key] = false;
		}else{
			relay.turnOnSocket(l, blocking);
			status[key] = true;
		}
	}

	if (key=='i'){ //set the LAN cfg of the device
		string cmd = "NEUNIT=1,SETTINGS=LAN,IPADDRESS=192.168.102.16,SUBNET=255.255.255.0,GATEWAY=192.168.102.1\\CR";
		relay.sendCmdBlocking(cmd);
	}

	if (key=='s'){ //save settings on the device
		string cmd = "NEUNIT=1,SAVE=TRUE\\CR";
		//string cmd = "NEUNIT=1,RELAY=1,ACTION=SET,TIME=0\\CR";
		//string cmd = "NEUNIT=1,RESET=TRUE\\CR";
		//string cmd = "NEUNIT=1,UNITID=1\CR";
		//string cmd = "NEUNIT=1,FACTORYDEFAULT=TRUE\\CR";

		relay.sendCmdBlocking(cmd);
	}

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
