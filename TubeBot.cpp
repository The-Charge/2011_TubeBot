#include "Gyro.h"
#include "WPILib.h"
#include "Sensors.h"
#include "rebootLib.h"
#include "CANJaguar.h"
#include "math.h"
#include "DashboardDataFormat.h"

//.06 addition to all but outside top
//PORT 6
//these constants change where the arm can move in certain positions
const float OFFSET=.094;//((10.0f/300.0f)+.06);
const int MAX_POS_0=210;
const int MAX_POS_1=300;
const int MAX_POS_2=300;
const int MAX_POS_3=300;
const float FOLD=(7.0f/300.0f);
const float MIDDLE_LOW_ANGLE=(.406)-OFFSET;//(120.0f/300.0f); 
const float MIDDLE_TOP_ANGLE=((.684)-OFFSET);
const float TOP_LOW_ANGLE=.836-OFFSET;//(240.0f/300.0f);
const float TOP_TOP_ANGLE=.760-OFFSET;//(247.0f/300.0f);
const float GROUND=(.6219);
const float POT_GAIN[6]={(60.0f/300.0f),(60.0f/300.0f),(60.0f/300.0f),(60.0f/300.0f),(20.0f/300.0f),0};
const bool LEFT=true;//constants for possible auton change;
const bool RIGHT=false;
const float OUTSIDE=.22;
const float INSIDE=.15;
const float CENTERY=60;
const float FORWARDY=139;
const float STRAIGHT=180;//was 170
const int CAMERA_FLOOR=55;
const int CAMERA_CENTER=85;
const float ARM_P=-1500;
int position;
int mode; //Autonomous, leave it for now.
bool compressorState; //True==On
bool armState;
#define _NEWCYP			// Must be defined
#define _CYPWRAP		// Define to use Joystick #2 for inputs
						//		Don't define to use Cypress Box for inputs

#ifndef _NEWCYP
class Cypress {			// This code wont compile under any configuration of _NEWCYP or _CYPWRAP
public:
	// \branches\auton
	// \trunk\2011bot
	DriverStation *m_ds;
	bool enhanced;
	/*pass it a true value if you need to use inhanced io, 
	 * ie if you have more than 4 analog ins or have rewired the cypress
	 */
	Cypress::Cypress(bool enhan) 
	{
		m_ds=DriverStation::GetInstance();
		enhanced=enhan;
	}

	bool Cypress::GetDigitalIn(int channel)

	{
		int getchan=abs(channel-8)+1;
		if (enhanced)
			return !m_ds->GetEnhancedIO().GetDigital(getchan);
		else
			return m_ds->GetDigitalIn(getchan);
	}

	void Cypress::SetDigitalOut(int channel, bool value) {
		int getchan=abs(channel-8)+1;
		if (enhanced) {
			getchan+=8;
			m_ds->GetEnhancedIO().SetDigitalOutput(getchan, value);
		} else
			m_ds->SetDigitalOut(getchan, value);
	}

	float Cypress::GetAnalogIn(int channel) {
		int getchan=abs(channel-4)+1;
		if (enhanced)
			return m_ds->GetEnhancedIO().GetAnalogIn(getchan);
		else
			return m_ds->GetAnalogIn(getchan);
	}
	bool Cypress::GetClaw()
	{
		return GetDigitalIn(3);
	}

};
#else
#ifndef _CYPWRAP
class Cypress {				// 
public:
	// \branches\auton
	// \trunk\2011bot
	DriverStation *m_ds;
	bool toggle;
	bool pressed;
	bool state1; //Middle Low
	bool state2; //Top Low
	bool state3; //Middle Top
	bool state4; //Top Top
	bool state5; //Ground
	bool state6; //Fold
	/*pass it a true value if you need to use inhanced io, ie if you have more than 4 analog ins or have rewired the cypress
	 */
	Cypress::Cypress(bool enhanced) {
		m_ds=DriverStation::GetInstance();
		toggle=false;
		pressed=false;
		state1=false;
		state2=false;
		state3=false;
		state4=false;
		state5=false;
		state6=true;
	}

	bool Cypress::GetDigitalIn(int channel)

	{
		
		
			return m_ds->GetEnhancedIO().GetDigital(channel);
		
	}

	void Cypress::SetDigitalOut(int channel, bool value) {
		
			
			m_ds->GetEnhancedIO().SetDigitalOutput(channel, value);
	
	}

	float Cypress::GetAnalogIn(int channel) {
		
			return m_ds->GetEnhancedIO().GetAnalogIn(channel);
			
		
	}
	void Cypress::Toggle()
	{
		if(GetDigitalIn(6)&&!pressed)
		{
			pressed=true;
			toggle=!toggle;
			SetDigitalOut(16,toggle);
			
		}
		else if(!GetDigitalIn(6))
			pressed=false;
		
		if(GetDigitalIn(4))
		{
			state1=true; //Middle Low
			state2=false;
			state3=false;
			state4=false;
			state5=false;
			state6=false;
		}
		else if(GetDigitalIn(2))
		{
			state1=false;
			state2=true;
			state3=false;
			state4=false;
			state5=false;
			state6=false;
		}
		else if(GetDigitalIn(3))
		{
			state1=false;
			state2=false;
			state3=true;
			state4=false;
			state5=false;
			state6=false;
		}
		else if(GetDigitalIn(1))
		{
			state1=false;
			state2=false;
			state3=false;
			state4=true;
			state5=false;
			state6=false;
		}
		else if(GetDigitalIn(5))
		{
			state1=false;
			state2=false;
			state3=false;
			state4=false;
			state5=true;
			state6=false;
		}
		else if(GetDigitalIn(7))
		{
			state1=false;
			state2=false;
			state3=false;
			state4=false;
			state5=false;
			state6=true;
		}
		/*
		if(GetDigitalIn(1) && state1)
		{
			state1=false;
		}
		else if(GetDigitalIn(2) && state2)
		{
			state2=false;
		}
		else if(GetDigitalIn(3) && state3)
		{
			state3=false;
		}
		else if(GetDigitalIn(4) && state4)
		{
			state4=false;
		}
		else if(GetDigitalIn(5) && state5)
		{
			state5=false;
		}*/
		
	}
	float Cypress::GetFineAdjust()
	{
		return 0;		// TODO: Add code for Cypress
	}
	/*this function was added speciffically for the 2011 compitition
	 * so that we could interchangeibly use the new/old cypress
	 * it is also being added to the other cypress class
	 */
	bool Cypress::GetClaw()
	{
		Toggle();
		return toggle;
	}
	int Cypress::getState()
	{
		if(state1)
			return 1;
		else if(state2)
			return 2;
		else if(state3)
			return 3;
		else if(state4)
			return 4;
		else if(state5)
			return 5;
		else if(state6)
			return 6;
		else
		{
			cout << "Your have no state... might want to get that checked out\n";
			return -1;
		}
	}
	bool Cypress::getSwitch(){
		return GetAnalogIn(5)>1.5;
	}
};
#else
class Cypress{			// Using Joystick unber 2 for "Cypress" inputs
public: 
	DriverStation *m_ds;
	Joystick *s2;
	bool toggle;
	bool pressed;
	bool state1; //Middle Low
	bool state2; //Top Low
	bool state3; //Middle Top
	bool state4; //Top Top
	bool state5; //Ground
	bool state6; //Fold
	Cypress::Cypress(bool enhanced)
	{
		m_ds=DriverStation::GetInstance();
		s2=new Joystick(2);
		toggle=false;
		pressed=false;
		state1=false;
		state2=false;
		state3=false;
		state4=false;
		state5=false;
		state6=true;
	}
	float Cypress::GetFineAdjust()
	{
		return ((s2->GetRawAxis(4)+1)/2)*3.3;
	}
	bool Cypress::GetClaw()
	{
		Toggle();
		return toggle;
	}
	void Cypress::Toggle()
	{
		if(s2->GetRawButton(6)&&!pressed)
		{
			pressed=true;
			toggle=!toggle;
			//SetDigitalOut(16,toggle);
			
		}
		else if(!s2->GetRawButton(6))
			pressed=false;
		
		if(s2->GetRawButton(4))
		{
			state1=true; //Middle Low
			state2=false;
			state3=false;
			state4=false;
			state5=false;
			state6=false;
		}
		else if(s2->GetRawButton(2))
		{
			state1=false;
			state2=true;
			state3=false;
			state4=false;
			state5=false;
			state6=false;
		}
		else if(s2->GetRawButton(3))
		{
			state1=false;
			state2=false;
			state3=true;
			state4=false;
			state5=false;
			state6=false;
		}
		else if(s2->GetRawButton(1))
		{
			state1=false;
			state2=false;
			state3=false;
			state4=true;
			state5=false;
			state6=false;
		}
		else if(s2->GetRawButton(5))
		{
			state1=false;
			state2=false;
			state3=false;
			state4=false;
			state5=true;
			state6=false;
		}
		else if(s2->GetRawButton(7))
		{
			state1=false;
			state2=false;
			state3=false;
			state4=false;
			state5=false;
			state6=true;
		}
		//trigger=top_top
		//
		//button a=ground(5)
		//button b=claw(6)
		//button c=7
			
	}
	int Cypress::getState()
	{
		if(state1)
			return 1;
		else if(state2)
			return 2;
		else if(state3)
			return 3;
		else if(state4)
			return 4;
		else if(state5)
			return 5;
		else if(state6)
			return 6;
		else
		{
			cout << "Your have no state... might want to get that checked out\n";
			return -1;
		}
	}
	
	bool Cypress::getSwitch(){
		return s2->GetRawAxis(5)>.7;
	}
};
#endif
#endif

class RawControl {
public:
	CANJaguar *frontRight, *frontLeft, *backRight, *backLeft, *arm;
	IRSensor *leftIR, *centerIR, *rightIR;
	DoubleSolenoid *low, *high;
	Solenoid *gripper;
	Solenoid *deploy;
	Joystick *stick;
	Gyro *m_Gyro;
	Compressor *comp;
	Servo *tilt;
	Cypress *m_Cypress;
	DigitalOutput *gyrocal;
	Encoder *wheelEncoderBL, *wheelEncoderBR, *wheelEncoderFL, *wheelEncoderFR;
	/*
	 Gyro *gyr;
	 Gyro *gyr2;
	 float x,y,z;
	 
	 float pos;
	 float spp;
	 */

	RawControl::RawControl() {
		frontLeft = new CANJaguar(1,CANJaguar::kSpeed);
		frontRight = new CANJaguar(2,CANJaguar::kSpeed);
		backLeft = new CANJaguar(3,CANJaguar::kSpeed);
		backRight = new CANJaguar(4,CANJaguar::kSpeed);
		arm = new CANJaguar(5,CANJaguar::kPosition);
		low=new DoubleSolenoid(5,6);
		high=new DoubleSolenoid(3,4);
		gripper=new Solenoid(8);
		deploy=new Solenoid(7);
		comp=new Compressor(4,1);
		comp->Start();
		compressorState=true;
		armState=true;
		configJags();
		tilt=new Servo(1);
		m_Cypress=new Cypress(true);
		//gyrocal=new DigitalOutput(5);
		//gyrocal->Pulse(.0015);
		Wait(1);
		
		m_Gyro = new Gyro(1);
		//m_Gyro->SetSensitivity(.002);
		
		//checkCan();
		//relay=1
		//switch 4

		leftIR = new IRSensor(1);
		centerIR = new IRSensor(2);
		rightIR = new IRSensor(3);//silly electrical team

		stick = new Joystick(1);
		//stick-new Joystick(1);
		//cout<<stick<<"\n";
		
		checkCan();
		//(427.0f/360)*(.002/3));//.000740569538394235);//.002

		wheelEncoderFL=new Encoder(7,8);
		wheelEncoderFR=new Encoder(13,14);
		wheelEncoderBL=new Encoder(11,12);
		wheelEncoderBR=new Encoder(9,10);
		wheelEncoderFR->Start();
		wheelEncoderFL->Start();
		wheelEncoderBR->Start();
		wheelEncoderBL->Start();
		wheelEncoderFR->Reset();
		wheelEncoderFL->Reset();
		wheelEncoderBR->Reset();
		wheelEncoderBL->Reset();
	}
	void RawControl::deployMini()
	{
		if(chkSwitch())
		{
			deploy->Set(true);
		}
		else
			deploy->Set(false);
	}
	void setMotors(float left,float right)
	{
		left=-left;
		right=-right;
		left*=300;
		right*=300;
		frontLeft->Set(-left);
		frontRight->Set(right);
		backRight->Set(right);
		backLeft->Set(-left);
		//CANJaguar::UpdateSyncGroup(2);
	}
	void RawControl::checkCan()
	{
		if(arm->GetP()!=ARM_P)
			reboot(0x04);
		
	}
	void RawControl::armAngle(float angle) {
		
		//angle/=300.0f;
		
		arm->Set(angle);
		//cout<<"angle= "<<angle<<"\n";
	}
	int RawControl::getState(){
		return m_Cypress->getState();
	}
		
	void RawControl::checkJag()
	{
		
		if(frontRight->GetFaults()!=0)
		{
			frontRight->ConfigNeutralMode(CANJaguar::kNeutralMode_Brake);
			frontRight->SetSafetyEnabled(false);
			frontRight->ConfigEncoderCodesPerRev(360);
			frontRight->EnableControl(0);

		}
		if(frontLeft->GetFaults()!=0)
		{
			frontLeft->ConfigNeutralMode(CANJaguar::kNeutralMode_Brake);
			frontLeft->SetSafetyEnabled(false);
			frontLeft->ConfigEncoderCodesPerRev(360);
			frontLeft->EnableControl(0);
			
		}
		if(backLeft->GetFaults()!=0)
		{
			backLeft->ConfigNeutralMode(CANJaguar::kNeutralMode_Brake);
			backLeft->SetSafetyEnabled(false);
			backLeft->ConfigEncoderCodesPerRev(360);
			backLeft->EnableControl(0);
			
		}
		if(backRight->GetFaults()!=0)
		{
			backRight->ConfigNeutralMode(CANJaguar::kNeutralMode_Brake);
			backRight->SetSafetyEnabled(false);
			backRight->ConfigEncoderCodesPerRev(360);
			backRight->EnableControl(0);
			
		}
		if(arm->GetFaults()!=0)
		{
			arm->SetPID(ARM_P, -.02, 0);
			arm->ConfigNeutralMode(CANJaguar::kNeutralMode_Brake);
			arm->SetSafetyEnabled(false);
			arm->ConfigMaxOutputVoltage(13);
			arm->SetPositionReference(CANJaguar::kPosRef_Potentiometer);
			arm->ConfigPotentiometerTurns(1);
			arm->EnableControl(0);
		}
		
		
	}
	void RawControl::resetJags()
	{
		{
			frontRight->ConfigNeutralMode(CANJaguar::kNeutralMode_Brake);
			frontRight->SetSafetyEnabled(false);
			frontRight->ConfigEncoderCodesPerRev(360);

			frontRight->EnableControl(0);

		}

		{
			frontLeft->ConfigNeutralMode(CANJaguar::kNeutralMode_Brake);
			frontLeft->SetSafetyEnabled(false);
			frontLeft->ConfigEncoderCodesPerRev(360);
			frontLeft->EnableControl(0);

		}

		{
			backLeft->ConfigNeutralMode(CANJaguar::kNeutralMode_Brake);
			backLeft->SetSafetyEnabled(false);
			backLeft->ConfigEncoderCodesPerRev(360);
			backLeft->EnableControl(0);

		}

		{
			backRight->ConfigNeutralMode(CANJaguar::kNeutralMode_Brake);
			backRight->SetSafetyEnabled(false);
			backRight->ConfigEncoderCodesPerRev(360);
			backRight->EnableControl(0);

		}
		frontLeft->ChangeControlMode(CANJaguar::kPercentVbus);
		frontRight->ChangeControlMode(CANJaguar::kPercentVbus);
		backLeft->ChangeControlMode(CANJaguar::kPercentVbus);
		backRight->ChangeControlMode(CANJaguar::kPercentVbus);
		frontLeft->ConfigFaultTime(0);
		backLeft->ConfigFaultTime(0);
		backRight->ConfigFaultTime(0);
		frontRight->ConfigFaultTime(0);

		frontLeft->EnableControl(0);
		frontRight->EnableControl(0);
		backLeft->EnableControl(0);
		backRight->EnableControl(0);

	}
	void RawControl::resetArm()
	{
		arm->SetPID(-5, 0, 0);
		arm->ConfigNeutralMode(CANJaguar::kNeutralMode_Brake);
		arm->SetSafetyEnabled(false);
		arm->SetPositionReference(CANJaguar::kPosRef_Potentiometer);
		arm->ConfigEncoderCodesPerRev(1);
		arm->EnableControl(0);
		
	}
	bool RawControl::chkSwitch()
	{//5
		if (m_Cypress->getSwitch())
			return true;
		else
			return false;
	}
	void RawControl::camAng(float angle)
	{
		tilt->Set(angle);
		//((tilt->GetMaxAngle())-(tilt->GetMinAngle()))/2;
	}
	void RawControl::armHeightPnu(bool one, bool two) {
		if (one) {
			low->Set(DoubleSolenoid::kForward);
		} else
			low->Set(DoubleSolenoid::kReverse);
		if (two) {
			high->Set(DoubleSolenoid::kForward);

		} else
			high->Set(DoubleSolenoid::kReverse);
	}
//CHECK THIS OUT!!
	void RawControl::configJags() {
		//will need to be tuned on the new robot
		frontLeft->SetPID(3, .07, 0);//tested values are 1,.02,0
		frontRight->SetPID(3, .09, 0);
		backLeft->SetPID(1, .013, 0);
		backRight->SetPID(1.2, .013, 0);
		arm->SetPID(ARM_P, -.02, 0);

		backLeft->ConfigNeutralMode(CANJaguar::kNeutralMode_Brake);
		frontLeft->ConfigNeutralMode(CANJaguar::kNeutralMode_Brake);
		frontRight->ConfigNeutralMode(CANJaguar::kNeutralMode_Brake);
		backRight->ConfigNeutralMode(CANJaguar::kNeutralMode_Brake);
		arm->ConfigNeutralMode(CANJaguar::kNeutralMode_Brake);
		//shoot everything remotely safety related
		backLeft->SetSafetyEnabled(false);
		backRight->SetSafetyEnabled(false);
		frontLeft->SetSafetyEnabled(false);
		frontRight->SetSafetyEnabled(false);
		arm->SetSafetyEnabled(false);
		
		
		

		frontLeft->ConfigMaxOutputVoltage(13);
		frontRight->ConfigMaxOutputVoltage(13);
		backLeft->ConfigMaxOutputVoltage(13);
		backRight->ConfigMaxOutputVoltage(13);
		arm->ConfigMaxOutputVoltage(13);

		frontLeft->SetSpeedReference(CANJaguar::kSpeedRef_Encoder);
		frontRight->SetSpeedReference(CANJaguar::kSpeedRef_Encoder);
		backRight->SetSpeedReference(CANJaguar::kSpeedRef_Encoder);
		backLeft->SetSpeedReference(CANJaguar::kSpeedRef_Encoder);
		arm->SetPositionReference(CANJaguar::kPosRef_Potentiometer);
		//not sure on these values either
		frontLeft->ConfigEncoderCodesPerRev(360);
		frontRight->ConfigEncoderCodesPerRev(360);
		backLeft->ConfigEncoderCodesPerRev(360);
		backRight->ConfigEncoderCodesPerRev(360);
		arm->ConfigPotentiometerTurns(1);
		frontLeft->ChangeControlMode(CANJaguar::kPercentVbus);
		frontRight->ChangeControlMode(CANJaguar::kPercentVbus);
		backLeft->ChangeControlMode(CANJaguar::kPercentVbus);
		backRight->ChangeControlMode(CANJaguar::kPercentVbus);
		frontLeft->EnableControl(0);
		frontRight->EnableControl(0);
		backLeft->EnableControl(0);
		backRight->EnableControl(0);
		frontLeft->ConfigFaultTime(0);
		backLeft->ConfigFaultTime(0);
		backRight->ConfigFaultTime(0);
		frontRight->ConfigFaultTime(0);
		arm->ConfigFaultTime(0);
		arm->EnableControl(0);
		arm->EnableControl(0);
		
		
		/*
		 fl=new CANJaguar(2,CANJaguar::kSpeed);
		 fr=new CANJaguar(3,CANJaguar::kSpeed);
		 bl=new CANJaguar(4,CANJaguar::kSpeed);
		 br=new CANJaguar(1,CANJaguar::kSpeed);
		 
		 fl->SetSpeedReference(CANJaguar::kSpeedRef_Encoder);
		 fr->SetSpeedReference(CANJaguar::kSpeedRef_Encoder);
		 br->SetSpeedReference(CANJaguar::kSpeedRef_Encoder);
		 bl->SetSpeedReference(CANJaguar::kSpeedRef_Encoder);
		 
		 fl->ConfigEncoderCodesPerRev(1440);
		 fr->ConfigEncoderCodesPerRev(1440);
		 bl->ConfigEncoderCodesPerRev(1440);
		 br->ConfigEncoderCodesPerRev(1440);
		 */
	}
	void RawControl::claw(bool open) {
		gripper->Set(m_Cypress->GetClaw());
	}
	int RawControl::zero() {
		arm->EnableControl(0);
		
		return 0;
	}

	/* gets stick values into the x y and z values, these values are not returned by this but rather stored into the respective variables, the values are filtered*/
	void RawControl::getStickValues(float &x, float &y, float &z) {
		x=(fabs(stick->GetRawAxis(1))<.3 ? 0 : stick->GetRawAxis(1));
		y=(fabs(stick->GetRawAxis(2))<.3 ? 0 : stick->GetRawAxis(2));
		z=(fabs(stick->GetRawAxis(3))<.3 ? 0 : stick->GetRawAxis(3));
		x=x*x*x*x*x*x*x;
		y=y*y*y*y*y*y*y;
		z*=.9;
		z=z*z*z;
		//y*=.9;
		//stick->GetRawButton(2) ? z=0 : z=z;

		//theres a lot more filtering of axes after this....but with comments on top of comments i dont remember what actually worked.  Any other 
		//manipulation of joystick input for x-y-z axes goes here.
	}
	float RawControl::averageEncoders()
	{
		float traveled;
		traveled=(((abs(wheelEncoderFR->Get()))+(abs(wheelEncoderFL->Get()))+(abs(wheelEncoderBR->Get()))+(abs(wheelEncoderBL->Get())))/4);
		traveled/=250;
		traveled=traveled * 8 * 3.14;	
		traveled=fabs(traveled);
		return traveled;
	}
	void RawControl::resetEncoders()
	{
		wheelEncoderFR->Reset();
		wheelEncoderFL->Reset();
		wheelEncoderBR->Reset();
		wheelEncoderBL->Reset();
		
	}
	float RawControl::getArmPos()
	{
		return arm->GetPosition();
	}
};

class FunctionBot {
public:
	RawControl *m_RawBot;
	float correct;
	
	
	int lpnulvl;
	RobotDrive *drive;
	Timer *arm;
	bool inTransit;
	float waitTill;
	FunctionBot::FunctionBot() {
		m_RawBot = new RawControl();
		drive= new RobotDrive(m_RawBot->frontLeft,m_RawBot->backLeft,m_RawBot->frontRight,m_RawBot->backRight);
		arm = new Timer();
		waitTill=0;
		configDrive();
		correct=0;

	}
	bool FunctionBot::IsOperatorControl()
	{
		return m_RawBot->m_Cypress->m_ds->IsOperatorControl();
	}
	void startTiming(float seconds)
	{
		waitTill=(arm->Get())+seconds;
	}
	bool timeExpired()
	{
		if((arm->Get())>waitTill)
			return true;
		else
			return false;
	}

	void FunctionBot::runArm()
	{
		int state;
		int pnulvl;
		float lim;
		state=getState();
		pnulvl=pnuState(state);
		if(state==5||state==6)
		{
			lim=MAX_POS_0/300.0f;
		}
		else
			lim=MAX_POS_1/300.0f;
		if(pnulvl>lpnulvl)
		{
			if(!inTransit)
			{
				updatePNU(pnulvl);
				inTransit=true;
				startTiming(1);
			}
			else if(timeExpired())
			{
				inTransit=false;
				lpnulvl=pnulvl;
				if(desiredAngle()<lim)
				{
					updateElbow(desiredAngle());
				}
				else
					updateElbow(lim);
			}
			else
			{
				if(desiredAngle()<MAX_POS_0/300.0f)
					updateElbow(desiredAngle());
				else
					updateElbow(MAX_POS_0/300.0f);
			}
		}
		else
		{
			if(currentAngle()>lim)
			{
				updateElbow(lim);
			}
			else
			{
				updatePNU(pnulvl);
				lpnulvl=pnulvl;
				if(desiredAngle()<lim)
					updateElbow(desiredAngle());
				else
					updateElbow(lim);
			}
		}
		updateClaw();
	}
	
	int FunctionBot::pnuState(int state)
	{
		if(state==5||state==6)
			return 0;
		else if(state==1)
			return 1;
		else if(state==3)
			return 2;
		else if (state==2||state==4)
			return 3;
		else 
			return 0;
	}
	int FunctionBot::getState()
	{
		return m_RawBot->getState();
	}
	float FunctionBot::getFineAdjustment(int state)
	{
		//TODO: cypress direct access here
		if(state!=0)
			return (((m_RawBot->m_Cypress->GetFineAdjust()-1.65)/1.65)*POT_GAIN[state-1]);
		else
			return 0;
	}
	float FunctionBot::desiredAngle()
	{
		/*
		 * (desired)+(pot()*gain)+offset
		*/
		//return (((m_FunctionBot->m_RawBot->m_Cypress->GetAnalogIn(1))/3.3));
		
		//TODO: cypress direct access here
		int state=getState();
		float desired;
	
		if(state==1)
			desired=(MIDDLE_LOW_ANGLE);
		else if(state==2)
		{
			desired=((float)TOP_LOW_ANGLE);
		}
		else if(state==3)
			desired=(MIDDLE_TOP_ANGLE);
		else if(state==4)
			desired=(TOP_TOP_ANGLE);
		else if(state==5)
			desired=GROUND; //130 is the max range in pos. 0 i.e. ground level.
		else if(state==6)
			desired=(FOLD);  //Fold position
		else
		{
			desired=0;
		}
		//cout << "desired = " << desired << endl;
		/*if(state!=5 && state!=6)
			state+=(((m_FunctionBot->m_RawBot->m_Cypress->GetAnalogIn(1)-1.65)/1.65)*.14); //40 /300(actual range of pot)  is equal to .13333...
		*/																		// ^desired range of pot
		
		desired+=getFineAdjustment(state);		
		desired+=OFFSET;
		
		if(desired>1)
		{
			//cout << "Desired=1\n";
			desired=1;
		}
		else if(desired<0)
		{
			//cout << "Desired==0\n";
			desired=0;
		}
		else
		{//cout << "Desired=" << desired << endl;
		}
		//cout<<desired<<endl;
		return desired;
	}
	float FunctionBot::currentAngle()
	{
		return m_RawBot->getArmPos();
	}
	void FunctionBot::updatePNU(int state)
	{
		if(state==0)
			m_RawBot->armHeightPnu(false,true);
		else if (state==1)
			m_RawBot->armHeightPnu(true,true);
		else if(state==2)
			m_RawBot->armHeightPnu(false,false);
		else if(state==3)
			m_RawBot->armHeightPnu(true,false);
		else
			m_RawBot->armHeightPnu(false,true);
	}
	/*
	 *range on angle is from 0-1 
	 *name is misleading
	 *fix later
	 */
	void FunctionBot::updateElbow(float angle)
	{
		m_RawBot->armAngle(angle);
	}
	/*this fuction will need to be updated for the final robot*/
	void FunctionBot::configDrive() {
		drive->SetInvertedMotor(RobotDrive::kFrontLeftMotor, true);
		drive->SetInvertedMotor(RobotDrive::kRearLeftMotor, true);
		//drive->SetInvertedMotor(RobotDrive::kFrontLeftMotor,true);
		drive->SetSafetyEnabled(false);
		//not sure on this value at all
		drive->SetMaxOutput(333);
	}


void FunctionBot::mecDrive(float cam) //tele-op function to drive using a joystick implementing RobotDrive->MecanumDrive_Cartesian function
	{
		float x, y, z;
		m_RawBot->getStickValues(x, y, z);
		if(m_RawBot->stick->GetRawButton(1))
		{
			z=z/3;
			x=x/2;
			y=y/2; 
		}
		drive->MecanumDrive_Cartesian(-x, -y,-z,0);
		//m_RawBot->camAng(-cam);
		if(m_RawBot->stick->GetRawButton(2))    //button that outputs current encoder reading on electric arm
		{
			m_RawBot->resetJags();
		}
		m_RawBot->checkJag();
		//cout<<m_RawBot->m_Gyro->GetAngle()<<"\n";
	}

	void FunctionBot::updateClaw() {
		m_RawBot->claw(m_RawBot->m_Cypress->GetClaw());

	}
	
	void FunctionBot::driveX(int x)
	{	
		m_RawBot->resetEncoders();
		correct=m_RawBot->m_Gyro->GetAngle();
		correct/=30;
		correct*=-1;
		float traveled;
		float traveledmode;
		traveled=m_RawBot->averageEncoders();
		traveledmode=m_RawBot->averageEncoders()+abs(x);
		//traveledmode=(traveledmode * 8 * 3.14) + abs(x);
		if(x<0)
		{
			while(abs(x)-m_RawBot->averageEncoders()>1&&!IsOperatorControl())
			{
				
				drive->MecanumDrive_Cartesian(0,-.15,0,0);//numbers might be wrong get checked
				traveled=m_RawBot->averageEncoders();
			
			}
		}
		else
		{
			while(abs(x)-m_RawBot->averageEncoders()>1&&!IsOperatorControl())
			{
				drive->MecanumDrive_Cartesian(0,.15,0,0);//numbers might be wrong get checked
				traveled=m_RawBot->averageEncoders();
						
			}	
		}
		drive->MecanumDrive_Cartesian(0,0,0,0);
		
		
	}
};
	
	


class MainRobotClass : public IterativeRobot {
public:
	bool follow;
	bool back;
	bool flag;
	//int mode = 0; // seclects which peg we score on in antonomous
	DriverStation *ds;
	bool cal;
	
	int shoulderLastPos,shoulderCommandPos;
	
	FunctionBot *m_FunctionBot;
	
	/*void pidfl(double p,double i,double d)
	 {
	 m_FunctionBot->m_RawBot->frontLeft->SetPID(p, i, 0);
	 
	 }
	 void pidbl(double p,double i,double d)
	 {
	 m_FunctionBot->m_RawBot->backLeft->SetPID(p, i, 0);
	 
	 }
	 void pidfr(double p,double i,double d)
	 {
	 m_FunctionBot->m_RawBot->frontRight->SetPID(p, i, 0);
	 
	 }
	 void pidbr(double p,double i,double d)
	 {
	 m_FunctionBot->m_RawBot->backRight->SetPID(p, i, 0);
	 
	 }*/

	MainRobotClass::MainRobotClass(void) {
		m_FunctionBot = new FunctionBot();
		GetWatchdog().SetEnabled(false);
		follow = true;
		back = true;
		cal=false;
		ds=DriverStation::GetInstance();
		flag=false;
		
	
	}


	
	
	
	

	void handleCyp()
{
	
	m_FunctionBot->m_RawBot->deployMini();//fix direct access
	m_FunctionBot->runArm();
	//masterArmControl(three,one,two);
}
	void MainRobotClass::RobotInit(void) {
		m_FunctionBot->m_RawBot->comp->Start();
	}
	void MainRobotClass::DisabledInit(void) {
	}
	void MainRobotClass::AutonomousInit(void) {
		
		m_FunctionBot->m_RawBot->m_Gyro->Reset();
		m_FunctionBot->m_RawBot->wheelEncoderFR->Start();
		m_FunctionBot->m_RawBot->wheelEncoderFL->Start();
		m_FunctionBot->m_RawBot->wheelEncoderBR->Start();
		m_FunctionBot->m_RawBot->wheelEncoderBL->Start();
		m_FunctionBot->m_RawBot->wheelEncoderFR->Reset();
		m_FunctionBot->m_RawBot->wheelEncoderFL->Reset();
		m_FunctionBot->m_RawBot->wheelEncoderBR->Reset();
		m_FunctionBot->m_RawBot->wheelEncoderBL->Reset();
		m_FunctionBot->m_RawBot->backLeft->ChangeControlMode(CANJaguar::kSpeed);
		m_FunctionBot->m_RawBot->backRight->ChangeControlMode(CANJaguar::kSpeed);
		m_FunctionBot->m_RawBot->frontLeft->ChangeControlMode(CANJaguar::kSpeed);
		m_FunctionBot->m_RawBot->frontRight->ChangeControlMode(CANJaguar::kSpeed);
		m_FunctionBot->m_RawBot->backLeft->SetSpeedReference(CANJaguar::kSpeedRef_Encoder);
		m_FunctionBot->m_RawBot->backRight->SetSpeedReference(CANJaguar::kSpeedRef_Encoder);
		m_FunctionBot->m_RawBot->frontLeft->SetSpeedReference(CANJaguar::kSpeedRef_Encoder);
		m_FunctionBot->m_RawBot->frontRight->SetSpeedReference(CANJaguar::kSpeedRef_Encoder);
		m_FunctionBot->m_RawBot->backLeft->EnableControl();
		m_FunctionBot->m_RawBot->backRight->EnableControl();
		m_FunctionBot->m_RawBot->frontLeft->EnableControl();
		m_FunctionBot->m_RawBot->frontRight->EnableControl();
		m_FunctionBot->drive->SetMaxOutput(333);
		 
	}
	void MainRobotClass::AutonomousPeriodic(void) 
	{
		
	}
void MainRobotClass::TeleopPeriodic(void) {
		
		back=true;
		follow=true;
		m_FunctionBot->mecDrive(0);
		handleCyp();

		if(m_FunctionBot->m_RawBot->stick->GetRawButton(12))    //button that outputs current encoder reading on electric arm
		{
			cout<<m_FunctionBot->m_RawBot->arm->GetPosition()<<"\n";
		}
}
	void MainRobotClass::TeleopInit(void) {
		m_FunctionBot->m_RawBot->backLeft->ChangeControlMode(CANJaguar::kPercentVbus);
		m_FunctionBot->m_RawBot->backRight->ChangeControlMode(CANJaguar::kPercentVbus);
		m_FunctionBot->m_RawBot->frontLeft->ChangeControlMode(CANJaguar::kPercentVbus);
		m_FunctionBot->m_RawBot->frontRight->ChangeControlMode(CANJaguar::kPercentVbus);
		m_FunctionBot->m_RawBot->backLeft->EnableControl();
		m_FunctionBot->m_RawBot->backRight->EnableControl();
		m_FunctionBot->m_RawBot->frontLeft->EnableControl();
		m_FunctionBot->m_RawBot->frontRight->EnableControl();
		m_FunctionBot->drive->SetMaxOutput(1);
		//m_FunctionBot->m_RawBot->m_Gyro->Reset();
		//m_FunctionBot->m_RawBot->armAngle(0);
		m_FunctionBot->arm->Start();
		flag=false;
	}
	
	void MainRobotClass::DisabledPeriodic(void) {
		
		if(m_FunctionBot->m_RawBot->stick->GetRawButton(3)&&m_FunctionBot->m_RawBot->stick->GetRawButton(4)&&m_FunctionBot->m_RawBot->stick->GetRawButton(5)&&m_FunctionBot->m_RawBot->stick->GetRawButton(6))
		{
			reboot(0x04);
		}
		cout<<m_FunctionBot->m_RawBot->m_Gyro->GetAngle()<<"\n";
		
		
	}
	void MainRobotClass::TeleopContinuous(void) {
	}
	void MainRobotClass::DisabledContinuous(void) {
	}
	void MainRobotClass::AutonomousContinuous(void) {
	}
};



START_ROBOT_CLASS(MainRobotClass);

