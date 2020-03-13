int speakerPin = 15;

int length = 51;                                                                                                // 노래의 총 길이 설정
char notes[] = "eeeeeeegcde fffffeeeeddedgeeeeeeegcde fffffeeeggfdc";             // 음계 설정
int beats[] = { 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 4,
                1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2,
                1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 4,
                1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4
              };                                                                                                           // 해당하는 음이 울리는 길이 설정

int tempo = 300;                                                                                                   // 캐럴이 연주되는 속도

void playTone(int tone, int duration)
{
  for (long i = 0; i < duration * 1000L; i += tone * 2)
  {
    digitalWrite(speakerPin, HIGH);
    delayMicroseconds(tone);
    digitalWrite(speakerPin, LOW);
    delayMicroseconds(tone);
  }
}

void playNote(char note, int duration) {
  char names[] = { 'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C' };                                                         //음계 함수 설정
  int tones[] = { 1915, 1700, 1519, 1432, 1275, 1136, 1014, 956 };                                 // 음계 톤 설정
  for (int i = 0; i < 8; i++)
  {
    if (names[i] == note)
    {
      playTone(tones[i], duration);
    }
  }
}

void setup()
{
  pinMode(speakerPin, OUTPUT);
}

void loop() {
  for (int i = 0; i < length; i++)
  {
    if (notes[i] == ' ')
    {
      delay(beats[i] * tempo); // rest
    }
    else
    {
      playNote(notes[i], beats[i] * tempo);
    }
    delay(tempo / 10);
  }
  delay(500);
}
