#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/Rand.h"
#include "cinderSyphon.h"
#include "AudioUnit.h"
#include "OscListener.h"


using namespace ci;
using namespace ci::app;
using namespace std;

#define WIDTH 512
#define HEIGHT 512

class ChormusikApp : public AppNative {
  public:
    void prepareSettings( Settings *settings );
	void setup();
	void mouseDown( MouseEvent event );
    void keyDown( KeyEvent event );
	void update();
	void draw();
    
    au::GenericUnit reverb;
    au::SpeechSynth speechSynth;
    au::Mixer mixer;
    au::Output  output;
    au::Tap tap;
    au::TapSampleBuffer sampleBuffer;
    au::GenericUnit bidule;
    
    
    gl::TextureRef	mLogo;
	gl::GlslProgRef	mShader;
	float			mAngle;
    
	syphonServer mScreenSyphon; //each item to publish requires a different server
	syphonServer mTextureSyphon;
    
	syphonClient mClientSyphon; //our syphon client
    
    
    
};

void ChormusikApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize(WIDTH,HEIGHT);
	settings->setFrameRate(60.f);
}


void ChormusikApp::setup()
{
    reverb = au::GenericUnit(kAudioUnitType_Effect, kAudioUnitSubType_MatrixReverb);
    // tryout working wirh bidule
    bidule = au::GenericUnit('aufx', 'BIDS', 'PLOG') ;
    speechSynth.connectTo(bidule).connectTo(tap).connectTo(mixer).connectTo(output);
    output.start();
    
    
    try {
		mLogo = gl::Texture::create( loadImage( loadAsset("cinder_logo_alpha.png") ) );
	}
	catch( ... ) {
		std::cout << "unable to load the texture file!" << std::endl;
	}
    
	try {
		mShader = gl::GlslProg::create( loadAsset("passThru_vert.glsl"), loadAsset("gaussianBlur_frag.glsl") );
	}
	catch( gl::GlslProgCompileExc &exc ) {
		std::cout << "Shader compile error: " << std::endl;
		std::cout << exc.what();
	}
	catch( ... ) {
		std::cout << "Unable to load shader" << std::endl;
	}
    
	mAngle = 0.0f;
    
	mScreenSyphon.setName("Chormusik Output"); // set a name for each item to be published
	mTextureSyphon.setName("Texture Output");
    
	mClientSyphon.setup();
    
	// in order for this to work, you must run simple server which is a syphon test application
    // feel free to change the app and server name for your specific case
    mClientSyphon.set("", "Simple Server");
    
    mClientSyphon.bind();
    
    
}

//void ChormusikApp::mouseDown( MouseEvent event )
//{
//    std::string hello("RevoluttttttttttttttttttionHörenRevolutttttttttttttttttttttioooooooooooootiooooooooootiooooooooootiooooooooootioooooooooonSagenRevolutionHörenSagen");
//        hello.append("HörenSaaaaaaaaaaaaaaggggggggggggggen ");
//        hello.append("Sagen Hören? Fragen - Sagen säge Sarg , Sagen Höre Revolution ! Soooooooooo oder So. Maidan Maindan Maindan Maidaaaaaaaaaaaaaaaaaaaaaaaaaaaaa nnnnnn gggggggggggggggggggg de de de de de dede de dede de de Maidan");
//    
//        speechSynth.say(hello);
//}




void ChormusikApp::mouseDown( MouseEvent event )
{
    std::string hello("lllllllllllllllllllltttttttttttttaaaaaaaddddddddddrrrrrrrrrrramoramoreamoremiomiomiomio amoremio amore mio lllllllllllllllllllltttttttttttttaaaaaaaddddddddddrrrrrrrrrrr");
//    hello.append("HörenSaaaaaaaaaaaaaaggggggggggggggen ");
//    hello.append("Sagen Hören? Fragen - Sagen säge Sarg , Sagen Höre Revolution ! Soooooooooo oder So. Maidan Maindan Maindan Maidaaaaaaaaaaaaaaaaaaaaaaaaaaaaa nnnnnn gggggggggggggggggggg de de de de de dede de dede de de Maidan");
    
    speechSynth.say(hello);
}

void ChormusikApp::keyDown( KeyEvent event )
{
	if(event.getChar() == 'r') {
		bidule.showUI();
	}
}

void ChormusikApp::update()
{
    // The mixer's pan setting should be between -1 and 1 (full left to full right)
	float pan = (getMousePos().x - getWindowPosX()) / (float)getWindowWidth();
	pan = (pan * 2) - 1;
	if(pan >  1) pan = 1;
	if(pan < -1) pan = -1;
	mixer.setPan(pan);
    //graphics ...
    
    mAngle += 0.05f;
}

void ChormusikApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
    
	// samples extracted from a tap will be in the range of -1 to 1
	tap.getSamples(sampleBuffer);
	PolyLine2f waveform;
	for(int i = 0; i < sampleBuffer.size(); i++) {
		float x = i / (float)sampleBuffer.size() * (float)getWindowWidth();
		float y = ((sampleBuffer[i] + 1) / 2.) * (float)getWindowHeight();
		waveform.push_back(Vec2f(x,y));
	}
    
	gl::draw(waveform);
    
    
    //syphon 
    
    //gl::enableAlphaBlending();
	//gl::clear(Color::white());
    //gl::color(ColorA(1.f, 1.f, 1.f, 1.f));
    
    //mShader->bind();
    //mShader->uniform( "tex0", 0 );
    //mShader->uniform( "sampleOffset", Vec2f( cos( mAngle ), sin( mAngle ) ) * ( 3.0f / getWindowWidth() ) );
    //gl::draw(mLogo, Vec2f::zero());
    //mShader->unbind();
    
    mClientSyphon.draw(Vec2f(16.f, 64.f)); //draw our client image
    
	mScreenSyphon.publishScreen(); //publish the screen's output
	//mTextureSyphon.publishTexture(mLogo); //publish our texture without shader
    
	//anything that we draw after here will not be published
//    gl::drawStringCentered("This text will not be published to Syphon.", Vec2f(getWindowCenter().x, 20.f), ColorA::black());
//    
//    gl::clear( Color( 0, 0, 0 ) );
//    
//	// samples extracted from a tap will be in the range of -1 to 1
//	tap.getSamples(sampleBuffer);
//	PolyLine2f waveform;
//	for(int i = 0; i < sampleBuffer.size(); i++) {
//		float x = i / (float)sampleBuffer.size() * (float)getWindowWidth();
//		float y = ((sampleBuffer[i] + 1) / 2.) * (float)getWindowHeight();
//		waveform.push_back(Vec2f(x,y));
//	}
//    
//	gl::draw(waveform);
}

CINDER_APP_NATIVE( ChormusikApp, RendererGl )
