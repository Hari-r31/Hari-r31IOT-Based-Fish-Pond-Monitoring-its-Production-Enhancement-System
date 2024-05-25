const int trigPin = 12;
const int echoPin = 13;
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701
long duration;
float distanceCm;
float distanceInch;
void ultrasonicSensor_init()
{
  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin, INPUT); 
}
int ultrasonicSensorReading()
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distanceCm = duration * SOUND_SPEED/2;
  distanceInch = distanceCm * CM_TO_INCH;
  return distanceCm;
}
