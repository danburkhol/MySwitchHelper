//Struct Defs for Switch433

struct SWITCH {
  String name;
  unsigned long on_code;
  unsigned long off_code;
  int bitlength;
  //int child_id;

};

void printTest() {

	Serial.println("Print Test Func");
}