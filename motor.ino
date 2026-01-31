void GetBTCommand();                   
void BluetoothPadTranslateCommand();   
void SimpleMapInput();                 
void ExecuteCommand_L298N();          

float ControllerInput[8];    
float MotorOutputs[4];      


void setup() {
  Serial.begin(9600);           

  pinMode(13, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(11, OUTPUT);

  pinMode(4, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);

}

void loop() {
  GetBTCommand('#', ControllerInput);  
  SimpleMapInput(MotorOutputs, ControllerInput);
  ExecuteCommand_L298N(MotorOutputs);
}

void BluetoothPadTranslateCommand(float * ans , String Input){
  int X;
  int Y;
  int S;
  int C = Input.substring(0,1).toInt();
  int start = 2;
  int count = 2;
  int i = 0;
  
  while(i<2){
    count ++;

    if(Input.substring(count,count+1)==","){
      if(i==0){
        X=Input.substring(start,count).toInt();
        start = count + 1;
        i++;
      }else if(i==1){
        Y=Input.substring(start,count).toInt();
        start/////////////////////////////////////////////////////////////////// = count + 1;
        i++;
        S=Input.substring(start,Input.length()).toInt();
      }
    }
  }


  float angle = 180.0*atan2(Y,X)/3.14;
  if(Y<0){
    angle+=360.0;
  }
  
  float power = sqrt((X*X)+(Y*Y));
  if(power>100){
    power = 100.0;
  }
  power = power * 2.55;

  ans[0]=angle;
  ans[1]=power;
  ans[2]=S;
  for(int i = 3; i<7; i++){
    if((C+3) == i){
      ans[i]=1;
    }else{
      ans[i]=0;
    }
  }  
}

void SimpleMapInput(float * ans, float * Input){
  float angle = Input[0];
  float power = Input[1];
  int L_direction;
  int R_direction;
  float L_speed;
  float R_speed;


  if(angle<=180 ||angle>=330){
    L_direction = 1;
  }else{
    L_direction = 0;
  }

  if(angle<=200){
    R_direction = 1;
  }else{
    R_direction = 0;
  }

  if(angle>=90 && angle<=270){
    L_speed = abs(((angle-90.0)/90.0)-1.0)*power; 
  }else if(angle<90){
    L_speed = (0.5+(angle/180.0))*power; 
  }else{
    L_speed = abs(0.7 + (((angle-360.0)/90.0)*1.7))*power; 
  }

  if(angle<=90){
    R_speed = (angle/90.0)*power;
  }else if(angle>=270){
    R_speed = abs((angle-360)/90)*power;
  }else if(angle>90 && angle<180){
    R_speed = (1-((angle-90)/180))*power;
  }else{
    R_speed = abs(0.7 - (((angle-180.0)/90.0)*1.7))*power;
  }


  ans[0] = (L_direction);
  ans[1] = (R_direction);
  ans[2] = (L_speed);
  ans[3] = (R_speed);
}


void ExecuteCommand_L298N(float * Command){
  
  if(Command[0]>0){
    digitalWrite(DIR_L_F, HIGH);
    digitalWrite(DIR_L_B, LOW);
  }else{
    digitalWrite(DIR_L_B, HIGH);
    digitalWrite(DIR_L_F, LOW);
  }

  if(Command[1]>0){
    digitalWrite(DIR_R_F, HIGH);
    digitalWrite(DIR_R_B, LOW);
  }else{
    digitalWrite(DIR_R_B, HIGH);
    digitalWrite(DIR_R_F, LOW);
  }
  
  analogWrite(PWM_L, Command[2]);
  analogWrite(PWM_R, Command[3]);
}