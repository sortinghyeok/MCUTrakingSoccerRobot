#include <SimpleTimer.h> 
SimpleTimer timer; 
const int trigPin = 38;
const int echoPin = 37;
void setup() { 
   Serial.begin(115200);   Serial1.begin(115200); 
   pinMode(trigPin, OUTPUT);
   pinMode(echoPin, INPUT);
   timer.setInterval(100, Uart1); 
   timer.setInterval(100, getDistance);
   pinMode(4, OUTPUT); pinMode(5, OUTPUT);
   pinMode(28, OUTPUT); pinMode(29, OUTPUT);
   pinMode(30, OUTPUT); pinMode(31, OUTPUT);

} 

int duration;
float distance;
unsigned char Uart0ProtocolPointer = 0;     // UDR0버퍼의 연속적인 데이터에 대한 점프 
unsigned char Uart0ReciveCheckEnd = 0;// UDR0의 내용의 마지막을 의미 
unsigned char Data[12] = {0,}; 
unsigned int X_center =0; //UART로 가져오는 X좌표
unsigned int Y_center =0; //UART로 가져오는 Y좌표
unsigned int SigNum = 1;
unsigned int area = 0; //X 길이와 Y길이를 곱해 만드는 사각형의 넓이

unsigned int x_loc = 0; //8비트 좌표가 255를 넘을 수 없다는 한계가 있으므로, curLoc과 조합하여 현재 x좌표를 갱신해 사용해야할 필요가 있음


void UartRxProtocol(){ 
   unsigned char Uart0_Data; 
   if(Serial.available()){ 
      Uart0_Data = Serial.read(); 
      // ex) aa 55 aa a3 01 01 00 bf 00 51 00 6e 00 24 00 00 00 55 

      switch(Uart0ProtocolPointer) {
      //---------------------------------------------------------------------
  
         case 0: if(0xaa == Uart0_Data ) { Uart0ProtocolPointer = 1; } 
                          else { Uart0ProtocolPointer = 0; } 
         break; 
         case 1: if(0x55 == Uart0_Data ){ Uart0ProtocolPointer = 10;     } 
                          else {  Uart0ProtocolPointer = 0; } 
         break; 
//---------------------------------------------------------------------
      // 핵심 데이터 byte aa a3 01 01 00 bf 00 51 00 6e 00 24
        case 10: Data[0] = Uart0_Data; Uart0ProtocolPointer = 11; break;
        case 11: Data[1] = Uart0_Data; Uart0ProtocolPointer = 12; break;
        case 12: Data[2] = Uart0_Data;Uart0ProtocolPointer = 13; break;
        case 13: Data[3] = Uart0_Data; Uart0ProtocolPointer = 14; break; //signature number
        case 14: Data[4] = Uart0_Data; Uart0ProtocolPointer = 15; break; //6
        case 15: Data[5] = Uart0_Data; Uart0ProtocolPointer = 16; break; //X좌표
        case 16: Data[6] = Uart0_Data; Uart0ProtocolPointer = 17; break; //8
        case 17: Data[7] = Uart0_Data; Uart0ProtocolPointer = 18; break; //Y좌표
        case 18: Data[8] = Uart0_Data; Uart0ProtocolPointer = 19; break; //10
        case 19: Data[9] = Uart0_Data; Uart0ProtocolPointer = 20; break; //11
        case 20: Data[10] = Uart0_Data; Uart0ProtocolPointer = 21; break; //12
        case 21: Data[11] = Uart0_Data; Uart0ProtocolPointer = 100; break; //13
    
      case 100: if(0x00 == Uart0_Data ) { Uart0ProtocolPointer = 101; } 
                         else { Uart0ProtocolPointer = 0; } 
      break; 
      case 101: if(0x00 == Uart0_Data ) { Uart0ProtocolPointer = 102; } 
                         else { Uart0ProtocolPointer = 0; } 
      break; 
      case 102: if(0x00 == Uart0_Data ) { Uart0ProtocolPointer = 103; } 
                         else {Uart0ProtocolPointer = 0;} 
      break; 
      case 103: if(0x55 == Uart0_Data) { 
                        Uart0ProtocolPointer = 0; Uart0ReciveCheckEnd = 1; } 
                        else { Uart0ProtocolPointer = 0; }
      break; 

      default: Uart0ProtocolPointer = 0; 
      break;
      } 
   } 
} 

void Serial_Main0(void) { 
   if(Uart0ReciveCheckEnd==1) { 
      Uart0ReciveCheckEnd = 0; 
      unsigned int curLoc = X_center; //바로 이전까지의 x좌표
      X_center = Data[5] ; //픽시 카메라로부터 갱신한 좌표
      Y_center = Data[7] ; 
      SigNum = Data[3];
      area = Data[9]*Data[11]; //x길이 * y길이
      if(curLoc > 230 && X_center <= 57 || x_loc > 222 && X_center <= 57){ //이전까지의 개체 위치가 충분히 우측 지점에 있었는데도 불구하고 현재 갱신된 좌표가 57이하라면, 오버플로우로 판단
        x_loc = (255 + X_center); //오버플로우시 다시 계산한 좌표
      }
      else
      {
        x_loc = X_center; //오버플로우 일어나지 않았을 경우의 정상 좌표
      }
      
     if(SigNum == 1)
      {
        if(distance >=5)
        {
          //28 LOW 전방, 30 HIGH 전방
          if (x_loc <= 90 && x_loc != 0) {  //좌회전
      
            //analogWrite(4, 35); digitalWrite(28, HIGH); digitalWrite(29, HIGH);
            //analogWrite(5, 25); digitalWrite(30, HIGH); digitalWrite(31, HIGH);
            analogWrite(4, 55); digitalWrite(28, LOW); digitalWrite(29, HIGH);
            analogWrite(5, 25); digitalWrite(30, LOW); digitalWrite(31, HIGH);
          }
          else if (x_loc > 90 && x_loc < 222 && x_loc != 0) { //전방 질주
            analogWrite(4, 75); digitalWrite(28, LOW); digitalWrite(29, HIGH);
            analogWrite(5, 75); digitalWrite(30, HIGH); digitalWrite(31, HIGH);
          }
          else if (x_loc >= 222) { //우회전
            //카메라 정상시
            //analogWrite(4, 25); digitalWrite(28, LOW); digitalWrite(29, HIGH);
            //analogWrite(5, 35); digitalWrite(30, LOW); digitalWrite(31, HIGH);
            analogWrite(4, 55); digitalWrite(28, HIGH); digitalWrite(29, HIGH);
            analogWrite(5, 25); digitalWrite(30, HIGH); digitalWrite(31, HIGH);
          }
          else if(x_loc == 0)
          {
            analogWrite(4, 45); digitalWrite(28, HIGH); digitalWrite(29, HIGH);
            analogWrite(5, 45); digitalWrite(30, HIGH); digitalWrite(31, HIGH);
          }
        }
        else //공을 가졌으며, 이제 다른 색체를 인식해야할 차례
        {
          analogWrite(4, 85); digitalWrite(28, LOW); digitalWrite(29, HIGH);
           analogWrite(5, 85); digitalWrite(30, HIGH); digitalWrite(31, HIGH);
        }
         
      }
      else//공이 카메라에서 사라짐은 공을 이미 가졌거나, 혹은 말 그대로 공의 위치를 파악 못한 것. 여기서 초음파 센서 이용
      {
        if(distance <=6)
        {
          if(SigNum == 2)
          {
            //28 LOW 전방, 30 HIGH 전방
          if (x_loc <= 90) {  //좌회전
            //카메라 정상시
            //analogWrite(4, 35); digitalWrite(28, HIGH); digitalWrite(29, HIGH);
            //analogWrite(5, 25); digitalWrite(30, HIGH); digitalWrite(31, HIGH);
            analogWrite(4, 55); digitalWrite(28, LOW); digitalWrite(29, HIGH);
            analogWrite(5, 35); digitalWrite(30, HIGH); digitalWrite(31, HIGH);
          }
          else if (x_loc > 90 && x_loc < 222) { //전방 질주
            analogWrite(4, 75); digitalWrite(28, LOW); digitalWrite(29, HIGH);
            analogWrite(5, 75); digitalWrite(30, HIGH); digitalWrite(31, HIGH);
          }
          else if (x_loc >= 222) { //우회전
   
            //analogWrite(4, 45); d `igitalWrite(28, LOW); digitalWrite(29, HIGH);
            //analogWrite(5, 5); digitalWrite(30, LOW); digitalWrite(31, HIGH);
            analogWrite(4, 55); digitalWrite(28, LOW); digitalWrite(29, HIGH);
            analogWrite(5, 35); digitalWrite(30, HIGH); digitalWrite(31, HIGH);
          }
          } 
          else //Distance가 5보다 큰데 sigNum이 2, 3이라는 것은 노란색을 잡지 못했는데 다른색만 보인다는 것. 후진하면서 회전하여 공인식 유도
          { //공이 벗어나지 않도록 천천히 후진하는게 최선이나 벽에 부딛히는 경우 다수이므로 회전으로 전환
             analogWrite(4, 25); digitalWrite(28, LOW); digitalWrite(29, HIGH);
             analogWrite(5, 25); digitalWrite(30, LOW); digitalWrite(31, HIGH);
          }
        }
        else
        {
          analogWrite(4, 25); digitalWrite(28, LOW); digitalWrite(29, HIGH);
         analogWrite(5, 25); digitalWrite(30, HIGH); digitalWrite(31, HIGH);
        }
             
           
      }
     
     
   } 
}


void Uart1(){ //UART통신을 통한 데이터 확인
   Serial1.print("X_center : ");
   Serial1.print(X_center); 
   Serial1.print(' '); 
   Serial1.print("Y_center : ");
   Serial1.print(Y_center); 
   Serial1.print(' '); 
   Serial1.print("Area : ");
   Serial1.print(area);
   Serial1.println(' ');
   Serial1.print("X_loc : ");
   Serial1.println(x_loc);
   Serial1.print("Signum : ");
   Serial1.println(SigNum);
} 
void getDistance(){ //초음파 센서를 이용한 거리 구하기
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distance = duration * 0.034/2;
    Serial1.println(duration);
    Serial1.print(" distance : ");
    Serial1.println(distance);
}
void loop() { 
   timer.run(); 
   UartRxProtocol(); 
   Serial_Main0();
  
}
