int red_light_pin= 11;
int green_light_pin = 10;
int blue_light_pin = 9;

int red = 0;
int green = 0;
int blue = 0;
int cpt;

void setup() {
  Serial.begin(9600);
  pinMode(red_light_pin, OUTPUT);
  pinMode(green_light_pin, OUTPUT);
  pinMode(blue_light_pin, OUTPUT);
  cpt = 0;

}
void loop() {
  //RGB_color(242,12,189);
  //RGB_color(red,green,blue);

}
void RGB_color(int red_light_value, int green_light_value, int blue_light_value)
 {
  analogWrite(red_light_pin, red_light_value);
  analogWrite(green_light_pin, green_light_value);
  analogWrite(blue_light_pin, blue_light_value);
}



 void serialEvent() {
   //char buffer[7];
   int tmp = 0;
   while (Serial.available()){
       //Serial.readBytesUntil(',', buffer, sizeof(int));
       //buffer[i] = 0;
       //red = atoi(buffer);
       if(cpt >= 2 || tmp >= 2) {
         cpt = 0;
         tmp = 0;
         //RGB_color(0,0,0);
         continue;
       }
         
       
       //if (cpt == 0)
         red = Serial.parseInt();
       //else if (cpt == 1)
         green = Serial.parseInt();
       //else if (cpt == 2) {
         blue = Serial.parseInt();
         RGB_color(red,green,blue);
       //}
         
       //R
       /*cpt ++;
       tmp ++;
       if(tmp >= 2 || cpt >= 2) {
         cpt = 0;
         tmp = 0;
         RGB_color(red,green,blue);
         delay(10);
         continue;
       }*/

   }
   
   Serial.print("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaWorks");
   

 }
