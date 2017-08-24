import controlP5.*;
import processing.serial.*;

Serial myPort;
ControlP5 cp5;
int redcolor;
color C1;

int courseNumber = 0;//ロボットが何コース目を走っているか
int COURSEMAX = 4; //コースの数
//int focusNumber = 1;//prosseingを使っている人がどこを選択しているか
int pidBW = 150, pidBH = 50;//ボタンの横縦
int pidBI = 20;//ボタン間隔
int textSize = 40;
int x, y;//ボタン等の座標

void setup () {
  size (950, 1000);
  String portName = "COM3"; //com5セット
  myPort = new Serial(this, portName, 9600);//開く
  cp5 = new ControlP5(this);//cp5の取得

  C1 = redcolor = color(255, 0, 0, 0);

  for (int i=1; i<COURSEMAX+1; i++) {
    x = y = pidBI; 
    y += (pidBH + pidBI) * i;
    x -= pidBW - pidBH;//番号だけ幅が小さいから
    x += pidBW + pidBI;//P
    cp5.addTextfield(str(i-1)+"P").setPosition(x, y).setSize(pidBW, pidBH).setFont(createFont("arial", 18)).setAutoClear(false);
    x += pidBW + pidBI;//I
    cp5.addTextfield(str(i-1)+"I").setPosition(x, y).setSize(pidBW, pidBH).setFont(createFont("arial", 18)).setAutoClear(false);
    x += pidBW + pidBI;//D
    cp5.addTextfield(str(i-1)+"D").setPosition(x, y).setSize(pidBW, pidBH).setFont(createFont("arial", 18)).setAutoClear(false);
    x += pidBW + pidBI;//時間
    cp5.addTextfield(str(i-1)+"T").setPosition(x, y).setSize(pidBW, pidBH).setFont(createFont("arial", 18)).setAutoClear(false);
    x += pidBW + pidBI;//スピード
    cp5.addTextfield(str(i-1)+"F").setPosition(x, y).setSize(pidBW, pidBH).setFont(createFont("arial", 18)).setAutoClear(false);
  }

  textLoad();//保存してあるのを読み取る

  x = y = pidBI; 
  x += (pidBH + pidBI); 
  y += (pidBH + pidBI)*(COURSEMAX+1);
  cp5.addButton("buttonSave").setLabel("save").setPosition(x, y)
    .setSize(pidBW*2+pidBI, pidBH).setFont(createFont("arial", 20));
  x += (pidBW + pidBI)*2;
  cp5.addButton("buttonSend").setLabel("send").setPosition(x, y)
    .setSize(pidBW*2+pidBI, pidBH).setFont(createFont("arial", 20));
}
void draw() {
  while ( myPort.available () > 0) { // If data is available, 
    btRead();
  }
  drawBackground();
}

void drawBackground() {
  x = y = pidBI;
  background(255);  // 背景色

  noFill(); //塗りつぶさない
  strokeWeight(5);
  rect(pidBI/2, pidBI/2 + (pidBI+pidBH)*(courseNumber+1), pidBH+pidBI+(pidBI+pidBW)*5, pidBI+pidBH);//現在走行中の部分を囲う

  fill(0);//テキストの再配置
  textSize(textSize);
  text("No", x, y + textSize);
  x -= pidBW - pidBH;//番号だけ幅が小さいから
  x += pidBW + pidBI;//P
  text("P", x + pidBW/2 - textSize/2, y + textSize);
  x += pidBW + pidBI;//I
  text("I", x + pidBW/2 - textSize/2, y + textSize);
  x += pidBW + pidBI;//D
  text("D", x + pidBW/2 - textSize/2, y + textSize);
  x += pidBW + pidBI;//直線の時間
  text("T", x + pidBW/2 - textSize/2, y + textSize);
  x += pidBW + pidBI;//スピード
  text("F", x + pidBW/2 - textSize/2, y + textSize);
  for (int i=1; i<COURSEMAX+1; i++) {
    x = y = pidBI; 
    y += (pidBH + pidBI) * i;
    text(str(i), x, y + textSize);
  }
}

void btRead() {//BTでの受け取り部分
  String readData = myPort.readString();
  println(readData); // read it and store it in val
  String[] s = splitTokens(readData, "_");
  if (s.length > 1) {
    if (s[0].equals("N")) {
      courseNumber = int(s[1]);
    }
  }
}

void controlEvent (ControlEvent theEvent) {
  switch(theEvent.getController().getName()) {
  case "buttonSave":
    //myPort.write (1) ; // send an H to indicate mouse is over square
    //theEvent.getController().setLabel("aho");
    textSave();
    textLoad();
    break;
  case "buttonSend"://ロボットへデータを転送
    textSave();//一度保存させる(文字などの不正データを省く)
    textLoad();//これで不正データが消える
    int sendCourseMax = 0,t=999;
    for(;sendCourseMax<COURSEMAX &&
         int(cp5.get(Textfield.class,str(sendCourseMax)+"T").getText())>0
        ;sendCourseMax++);
    if(sendCourseMax > 0){
      //println("シナリオが"+sendCourseMax+"個を送信します");
      //myPort.write("M_"+sendCourseMax);//まず、シナリオの個数を送る
      for(int i=0;i<sendCourseMax && 1==0;i++){
        myPort.write("No"+i+
        　"P"+int(cp5.get(Textfield.class, str(i)+"P").getText())+
          "I"+int(cp5.get(Textfield.class, str(i)+"I").getText())+
          "D"+int(cp5.get(Textfield.class, str(i)+"D").getText())+
          "T"+int(cp5.get(Textfield.class, str(i)+"T").getText())+
          "F"+int(cp5.get(Textfield.class, str(i)+"F").getText()));
      }//No番号P数値I数値D数値T数値F数値　のフォーマットで送信
    }else{
      println("シナリオが0個です\n送信できません"); 
    }
    break;
  }
}

void textSave() {
  //println("ev3setting保存開始");
  PrintWriter output;// 初期化
  output = createWriter("ev3setting.txt"); 
  for (int i=0; i<COURSEMAX; i++) {
    String s;
    s=cp5.get(Textfield.class, str(i)+"P").getText(); s=(s.equals("") || (match(s,"[^0-9\\.]") != null))?"0":s; 
    output.print(str(float(s))+",");
    s=cp5.get(Textfield.class, str(i)+"I").getText(); s=(s.equals("") || (match(s,"[^0-9\\.]") != null))?"0":s; 
    output.print(str(float(s))+",");
    s=cp5.get(Textfield.class, str(i)+"D").getText(); s=(s.equals("") || (match(s,"[^0-9\\.]") != null))?"0":s; 
    output.print(str(float(s))+",");
    s=cp5.get(Textfield.class, str(i)+"T").getText(); s=(s.equals("") || (match(s,"[^0-9\\.]") != null))?"0":s; 
    if(float(s) > 100) s = "100"; output.print(str(float(s))+",");
    s=cp5.get(Textfield.class, str(i)+"F").getText(); s=(s.equals("") || (match(s,"[^0-9]") != null))?"0":s;  
    if(int(s) > 100) s = "100"; output.println(str(int(s))+",");
  }
  output.flush(); // Writes the remaining data to the file
  output.close(); // Finishes the file
  //println("保存終了");
}
void textLoad() {
  //println("ev3setting読み取り開始");
  String lines[];
  lines = loadStrings("ev3setting.txt");
  for (int i=0; i<lines.length && i<COURSEMAX; i++) {
    //println(lines[i]);//読み取りデバック
    String[] s = splitTokens(lines[i], ",");
    cp5.get(Textfield.class, str(i)+"P").setText(s.length>0 ? s[0]:"0" );
    cp5.get(Textfield.class, str(i)+"I").setText(s.length>1 ? s[1]:"0" );
    cp5.get(Textfield.class, str(i)+"D").setText(s.length>2 ? s[2]:"0" );
    cp5.get(Textfield.class, str(i)+"T").setText(s.length>3 ? s[3]:"0" );
    cp5.get(Textfield.class, str(i)+"F").setText(s.length>4 ? s[4]:"0" );
  }
  //println("読み取り終了");
}


void keyPressed() {
  //background(0);
  println("key pressed key=" + key);
  float x = 200;
  float y = 100;
  //text(key, x, y);
}