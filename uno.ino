#include <Wire.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Fingerprint.h>

// #2 yellow, #3 white
SoftwareSerial mySerial(2, 3);// RX, TX

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
LiquidCrystal_I2C lcd(0x27, 16, 2);

int buttonPin = 13;
const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {{'1','2','3'}, {'4','5','6'}, {'7','8','9'}, {'*','0','#'}};
byte rowPins[ROWS] = {12, 11, 10, 9}; //row
byte colPins[COLS] = {8, 7, 6}; //column

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

int check = -1; //check mo cua
int col = 0; //cot hien thi LCD
int fingerID = -1; //id van tay
int passID = -1; //id mk so
bool remote = 0; //bien check remote
String password[5] = {"1234"}; //mang luu 5 password

int cnt_psw = 0;
int cnt_finger = 2;
bool finger_deleted[20] = {};
String dynamicPass = "";
String enteredPassword = "";
int cnt_wrongPass = 0;
String data = "";

void setup(){
  pinMode(buttonPin, INPUT);
  Serial.begin(9600);  
  Wire.begin();
  finger.begin(57600);
  // Khởi tạo màn hình LCD
  lcd.begin(16, 2);  // 16x2: 16 cột và 2 dòng
  for(int i = 0; i < 20; i++){
    finger_deleted[i] = 0;
  }
}

int menu_setting(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("1. Create user");
  lcd.setCursor(0, 1);
  lcd.print("2. Delete user");
  while(true){
    char key = keypad.getKey();
    if(key){
      lcd.clear();
      if(key == '1' || key == '2')
        return key - '0';
      else
        return 0;
    }
  }
}

int menu_add_delete(int type){
  lcd.clear();
  lcd.setCursor(0, 0);
  if(type == 1){
    lcd.print("1. New password");
    lcd.setCursor(0, 1);
    lcd.print("2. New fingerpr");
  }
  else if(type == 2){
    lcd.print("1. Del password");
    lcd.setCursor(0, 1);
    lcd.print("2. Del fingerpr");
  }
  while(true){
    char key = keypad.getKey();
    if (key) {
      lcd.clear();
      if(key == '1' || key == '2')
        return key - '0';
      else
        return 0;
    }
  }
}

int confirm(String pass, int miss_id){
  String entered = "";
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Re-enter:");
  int col_ = 0;
  while(true){
    char key = keypad.getKey();
    if (key) {
      if (key == '#') {
        lcd.clear();
        if(entered == "")
          return -1;
        if(entered == pass){
          if(miss_id != -1)
            password[miss_id] = entered;
          else{
            if(cnt_psw == 0){
              Serial.print('6'); // gui mk admin moi qua wemos
              Serial.print(entered);
            }
            password[cnt_psw] = entered;
            cnt_psw++;
          }
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("ID #"); 
          if(miss_id == -1)
            lcd.print(cnt_psw);
          else
            lcd.print(miss_id + 1);
          lcd.print(" added");
          lcd.setCursor(0, 1);
          lcd.print("Successfully");
          delay(2000);
          lcd.clear();
          return 1;
        }
        else
          return 0;
      }
      else if (key == '*'){
        lcd.clear();
        return -1;
      }
      else{
        lcd.setCursor(col_, 1);
        lcd.print('*');
        col_++;
        entered += key;
      }
    }
  }
  return -1;
}

void addPassword(){
  lcd.clear();
  if (cnt_psw == 5) {
    lcd.setCursor(0, 0);
    lcd.print("Fully stored!");
    delay(2000);
    lcd.clear();
    return;
  }
  int miss_id = -1;
  for(int i = 1; i < cnt_psw; i++){
    if(password[i] == ""){
      miss_id = i;
      break;
    }
  }
  String entered = "";
  lcd.setCursor(0, 0);
  lcd.print("ID #");
  if(miss_id == -1)
    lcd.print(cnt_psw + 1);
  else
    lcd.print(miss_id + 1);
  lcd.print(" password:");
  int col_ = 0;
  while(true){
    char key = keypad.getKey();
    if (key) {
      if (key == '#') {
        lcd.clear();
        if(entered == "")
          return;
        if(confirm(entered, miss_id) == 0){
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Confirm failed");
          delay(2000);
          lcd.clear();
        }
        return;
      }
      else if(key == '*'){
        lcd.clear();
        return;
      }
      else{
        lcd.setCursor(col_, 1);
        lcd.print('*');
        col_++;
        entered += key;
      }
    }
  }
}

void deletePassword(){
  int id = -1;
  if(cnt_psw > 1){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Enter ID: ");
    char entered = "";
    while(true){
      char key = keypad.getKey();
      if (key) {
        if (key == '#') {
          if(entered == "")
            break;
          id = entered - '0';
          if (id <= 1 || id > cnt_psw){
            id = -1;
          }
          break;
        }
        else if (key == '*'){
          break;
        }
        else{
          lcd.setCursor(0, 1);
          lcd.print(key);
          entered = key;
        }
      }
    }
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  if(id == -1)
    lcd.print("Cannot delete!");
  else{
    lcd.print("Are you sure?");
    lcd.setCursor(0, 1);
    lcd.print("1. Yes   2. No");
    while(true){
      char key = keypad.getKey();
      if(key){
        lcd.clear();
        if(key != '1')
          return;
        else
          break;
      }
    }
    password[id - 1] = "";
    lcd.print("Deleted ID #");
    lcd.print(id);
  }
  delay(2000);
  lcd.clear();
}

int checkAdmin(){
  String entered = "";
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Admin password:");
  col = 0;
  while(true){
    char key = keypad.getKey();
    if (key) {
      if (key == '#') {
        lcd.clear();
        col = 0;
        if(entered == "")
          return -1;
        if(entered == password[0])
          return 1;
        return 0;
      }
      else if(key == '*'){
        lcd.clear();
        return -1;
      }
      else{
        lcd.setCursor(col, 1);
        lcd.print('*');
        col++;
        entered += key;
      }
    }
  }
  return -1;
}

int getKeypad(int &check){
  char key = keypad.getKey();
  if (key) {
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Enter password:");
    if (key == '#') {
      lcd.clear();
      check = 0;
      if(enteredPassword == ""){
        check = -1;
        return -1;
      }
      else if(enteredPassword == "*0"){
        check = 2;
        return -1;
      }
      else if(enteredPassword == dynamicPass){
        check = 1;
        return -2;
      }
      else if(cnt_psw == 0 && enteredPassword == password[0]){
        check = 1;
        return 1;
      }
      else{
        for(int i = 0; i < cnt_psw; i++){
          if(enteredPassword == password[i]){
            check = 1;
            return i + 1;
          }
        }
      }
    }
    else if(key == '*' && enteredPassword == "*"){
      lcd.clear();
      col = 0;
      lcd.noBacklight();
      enteredPassword = "";
      return -1;
    }
    else if(key == '*' && enteredPassword != ""){
      lcd.clear();    
      lcd.setCursor(0, 0);
      lcd.print("Enter password:");
      col = 0;
      enteredPassword = "";
      return -1;
    }
    else{
      lcd.setCursor(col, 1);
      lcd.print('*');
      col++;
      if(col == 16)
        col = 0;
      enteredPassword += key;
    }
  }
  return -1;
}

int findFingerprint(int &check){
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;
  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;
  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK){
    check = 0;
    return -1;
  }
  check = 1;
  return finger.fingerID; 
}

void addFingerprint() {
  lcd.clear();
  if (cnt_finger == 20) {
    lcd.setCursor(0, 0);
    lcd.print("Fully stored!");
    delay(2000);
    lcd.clear();
    return;
  }
  int id = cnt_finger;
  for(int i = 0; i <= cnt_finger; i++){
    if(finger_deleted[i]){
      id = i;
      break;
    }
  }
  lcd.setCursor(0, 0);
  lcd.print("Enrolling ID #");
  lcd.print(id);
  while (!getFingerprintEnroll(id));
}

void getImage(Adafruit_Fingerprint &finger){
  int p = -1;
  int col_ = 0;
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      lcd.clear();
      delay(500);
      break;
    case FINGERPRINT_NOFINGER:
      lcd.setCursor(col_, 1);
      lcd.print(".");
      col_++;
      if(col_ == 16)
        col_ = 0;
    }
  }
}

int image2Tz(Adafruit_Fingerprint &finger, int time){ //lan 1 lan 2
  int p = finger.image2Tz(time);
  switch (p) {
    lcd.setCursor(0, 0);
    case FINGERPRINT_OK:
      delay(500);
      return p;
    case FINGERPRINT_IMAGEMESS:
      lcd.print("Image too messy");
      delay(1500);
      return p;
    case FINGERPRINT_FEATUREFAIL:
    case FINGERPRINT_INVALIDIMAGE:
      lcd.print("Couldn't convert");
      delay(1500);
      return p;
  }
}

uint8_t getFingerprintEnroll(int id) {
  getImage(finger);
  image2Tz(finger, 1);
  int p = -1;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Remove finger");
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("Place same"); 
  lcd.setCursor(0, 1); lcd.print("finger again");
  delay(2800);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enrolling");

  getImage(finger);
  image2Tz(finger, 2);

  lcd.clear();
  lcd.setCursor(0, 0);

  p = finger.createModel();
  if (p == FINGERPRINT_ENROLLMISMATCH) {
    lcd.print("Did not match");
    delay(2000);
    lcd.clear();
    return p;
  }

  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    lcd.setCursor(0, 0);
    lcd.print("ID #"); lcd.print(id); lcd.print(" added");
    lcd.setCursor(0, 1); lcd.print("Successfully");
    cnt_finger++;
    finger_deleted[id] = 0;
    delay(2000);
    lcd.clear();
  }
  return 1;
}

int delete_model(int id) {
  int p = -1;
  p = finger.deleteModel(id);
  if (p == FINGERPRINT_OK)
    return 1;
  return -1;
}

void deleteFingerprint(){
  int id = -1;
  if(cnt_finger >= 1){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Enter ID: ");
    String entered = "";
    int col_ = 0;
    while(true){
      char key = keypad.getKey();
      if (key) {
        if (key == '#') {
          if(entered == "")
            break;
          if (entered.toInt() >= 1 && entered.toInt() <= cnt_finger){
            id = entered.toInt();
          }
          break;
        }
        else if (key == '*'){
          break;
        }
        else{
          lcd.setCursor(col_, 1);
          lcd.print(key);
          col_++;
          entered += key;
        }
      }
    }
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  if(id != -1){
    lcd.print("Are you sure?");
    lcd.setCursor(0, 1);
    lcd.print("1. Yes   2. No");
    while(true){
      char key = keypad.getKey();
      if(key){
        lcd.clear();
        if(key != '1')
          return;
        else
          break;
      }
    }
    if(delete_model(id) == 1){
      finger_deleted[id] = 1;
      lcd.print("Deleted ID #");
      lcd.print(id);
    }
    else
      lcd.print("Cannot delete!");
  }   
  else
    lcd.print("Cannot delete!");
  delay(2000);
  lcd.clear();
}

void buttonPress(int &check){
  if (digitalRead(buttonPin) == HIGH){
    while(digitalRead(buttonPin) == HIGH);
    check = 1;
  }
}

void setting_(){
  lcd.clear();
  lcd.setCursor(0, 0);
  if(checkAdmin() == 1){ //check mk admin
    cnt_wrongPass = 0;
    int choice_setting = menu_setting();
    while(true){ //menu setting
      if(choice_setting == 1){ //them user
        int choice_add = menu_add_delete(choice_setting);
        while(true){
          if(choice_add == 1){
            addPassword();
          }
          else if (choice_add == 2){
            addFingerprint();
          }
          else{
            lcd.clear();
            break;
          }
          lcd.clear();
          choice_add = menu_add_delete(choice_setting);
        }
      }
      else if(choice_setting == 2){ //xoa user
        int choice_delete = menu_add_delete(choice_setting);
        while(true){
          if(choice_delete == 1){
            deletePassword();
          }
          else if (choice_delete == 2){
            deleteFingerprint();
          }
          else{
            lcd.clear();
            break;
          }
          lcd.clear();
          choice_delete = menu_add_delete(choice_setting);
        }
      }
      else{
        lcd.clear();
        break;
      }
      lcd.clear();
      choice_setting = menu_setting();
    }
  }
  col = 0;
  enteredPassword = "";
  check = -1;
}

void unlock(){
  lcd.clear();
  lcd.setCursor(0, 0);
  if(check == 1){ //mo cua
    if(passID == -1 && passID != -2 && fingerID == -1 && !remote){ // mo bang button
      Serial.print('7');
      cnt_wrongPass = 0;
    }
    else{
      lcd.backlight();
      lcd.print("Unlocked by");
      lcd.setCursor(0, 1);
      if(remote){ // mo tu xa
        lcd.print("remote");
        remote = 0;
      }
      else if(fingerID != -1){ // mo bang van tay
        Serial.print('2'); // gui tin hieu cho wemos
        Serial.print(fingerID);
        lcd.print("finger ID #");
        lcd.print(fingerID);
        fingerID = -1;
      }
      else if(passID == -2){ //mo bang dynamic pass
        Serial.print('3'); // gui tin hieu cho wemos
        lcd.print("dynamic password");
        passID = -1;
      }
      else if(passID != -1){ // mo bang mk so
        Serial.print('1'); // gui tin hieu cho wemos
        Serial.print(passID);
        lcd.print("password ID #");
        lcd.print(passID);
        passID = -1;
      }
      cnt_wrongPass = 0; // set lai so lan truy cap sai
      delay(5000);
      lcd.noBacklight();
    }
  }
  else if(check == 0){ //check sai
    cnt_wrongPass++;
    lcd.backlight();
    lcd.print("Verify fault!!");
    if(cnt_wrongPass == 5){
      lcd.clear();
      Serial.print('5'); // sai 5 lan
      lcd.setCursor(0, 0);
      lcd.print("Cannot access");
      lcd.setCursor(0, 1);
      lcd.print("now!!!!");
      delay(15000);
      lcd.noBacklight();
      cnt_wrongPass = 0;
    }
    else{ // sai < 5 lan
      Serial.print('4');
      delay(1500);
      lcd.noBacklight();
    }
  }
  lcd.clear();
  col = 0;
  enteredPassword = "";
  check = -1;
}

void loop() {
  if(Serial.available()){ // wemos truyen data
    data = Serial.readString();
    if(data == "Wifi"){
      lcd.backlight();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Wifi connected");
      delay(3000);
      lcd.clear();
      lcd.noBacklight();
    }
    else if(data == "Remote"){ // mo tu xa
      check = 1;
      remote = 1;
    }
    else if(data == "0") // het hieu luc dynamic
      dynamicPass = "";
    else // dynamic pass
      dynamicPass = data;
    data = "";
  }
  passID = getKeypad(check); //lay mat khau hoac '*0#'
  fingerID = findFingerprint(check); //check van tay
  buttonPress(check);
  if (check == 2) // setting
    setting_();
  else if(check != -1) // mo cua hoac nhap sai mk
    unlock();
}