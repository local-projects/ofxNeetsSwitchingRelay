#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    relay.setup("192.168.254.252",5000);
}

//--------------------------------------------------------------
void ofApp::update(){

    if(relay.client.isConnected()){
    
        if(!isConnected){
            cout<<"connected"<<endl;
            isConnected = true;
        }
        string str = relay.client.receive();
        if(str.size()){
            cout<<str<<endl;
        }
    }else{
        isConnected = false;
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofDrawBitmapString("Use 1-4 keys to turn on/off relay", 20,40);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){


	if(key >= '1' && key <= '4' ){
		int l = key - '1' + 1; //1..4
		if(status[key]){
			relay.turnOffSocket(l);
			status[key] = false;
		}else{
			relay.turnOnSocket(l);
			status[key] = true;
		}
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
