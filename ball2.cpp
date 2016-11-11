#include <GL/freeglut.h>
#include <math.h>

#define WIDTH 640
#define HEIGHT 480
#define PI 3.141592
#define ball_e 0.8
#define wall_e 1.0
#define floor_e 0.995

//回転用
float angle = 0.0f;
float roll = 0.0f;
//平行移動用
float zoom = 50.0f;
bool flag = false;

//ボールの色定義
GLfloat yellow[] = { 1.0, 1.0, 0.0, 1.0 };//黄
GLfloat lyellow[] = { 1.0, 1.0, 0.3, 1.0 };//淡黄色 
GLfloat blue[] = { 0.0, 0.0, 1.0, 1.0 };//青
GLfloat red[] = { 1.0, 0.0, 0.0, 1.0 };//赤
GLfloat purple[] = { 1.0, 0.0, 1.0, 1.0 };//紫
GLfloat orange[] = { 1.0, 0.7, 0.3, 1.0 };//橙
GLfloat green[] = { 0.0, 1.0, 0.0, 1.0 };//緑
GLfloat limelight[] = { 0.5, 1.0, 0.5, 1.0 };//緑
GLfloat brown[] = { 0.5, 0.4, 0.2, 1.0 };//茶
GLfloat black[] = { 0.1, 0.1, 0.1, 1.0 };//黒
GLfloat white[] = { 1.0, 1.0, 1.0, 1.0 };//白
GLfloat pink[] = { 1.0, 0.5, 0.5, 1.0 };//ピンク
//ライトの位置
GLfloat lightpos[] = { 0.0, 0.0, 100.0, 1.0 };


//3つのベクトル
struct Vector3f{
 float x;
 float y;
 float z;
}vec3d;

//球構造体
struct Sphere{
 GLfloat Color[4];//色
 GLfloat Radius;//半径
 Vector3f Pos;//位置
 Vector3f Speed;//速度
 bool flag;
 void Draw();
};

void Sphere::Draw(){
 glMaterialfv(GL_FRONT, GL_DIFFUSE, Color);
 glTranslatef(Pos.x,Pos.y,Pos.z);
 glutSolidSphere(Radius,16,16);
}

Sphere balls[10];

//ポケット構造体
struct Hole{
 GLfloat Color[4];//色
 GLfloat Radius;//半径
 Vector3f Pos;//位置
 void Draw();
};

//円柱
void cylinder(float radius,float height,int sides)
{
 //上面
 glNormal3d(0.0, 0.0, 1.0);
 glBegin(GL_POLYGON);
 for(double i = 0; i < sides; i++) {
  double t = PI*2/sides * (double)i;
  glVertex3d(radius * cos(t), radius * sin(t), height);
 }
 glEnd();
 //側面
 glBegin(GL_QUAD_STRIP);
 for(double i = 0; i <= sides; i++){
  double t = i*2*PI/sides;
  glNormal3f((GLfloat)cos(t), (GLfloat)sin(t), 0.0);
  glVertex3f((GLfloat)(radius*cos(t)), (GLfloat)(radius*sin(t)), -height);
  glVertex3f((GLfloat)(radius*cos(t)), (GLfloat)(radius*sin(t)), height);
 }
 glEnd();
 //下面
 glNormal3d(0.0, 0.0, -1.0);
 glBegin(GL_POLYGON);
 for(double i = sides; i >= 0; --i) {
  double t = PI*2/sides * (double)i;
  glVertex3d(radius * cos(t), radius * sin(t), -height);
 }
 glEnd();
}

void Hole::Draw(){
 glMaterialfv(GL_FRONT, GL_DIFFUSE, Color);
 glTranslatef(Pos.x,Pos.y,Pos.z);
 cylinder(Radius,6.0f,10.0f);
}

Hole pocket[6];

//当り判定
bool Calc_Hit(Sphere& a,Sphere& b){
 if(sqrt((a.Pos.x-b.Pos.x)*(a.Pos.x-b.Pos.x)+(a.Pos.y-b.Pos.y)*(a.Pos.y-b.Pos.y)+(a.Pos.z-b.Pos.z)*(a.Pos.z-b.Pos.z))<=a.Radius+b.Radius){
  return true;
 }else{
  return false;
 }
}

//消去判定
bool Calc_Erase(Sphere& a,Hole& b){
 if(sqrt((a.Pos.x-b.Pos.x)*(a.Pos.x-b.Pos.x)+(a.Pos.y-b.Pos.y)*(a.Pos.y-b.Pos.y)+(a.Pos.z-b.Pos.z)*(a.Pos.z-b.Pos.z))<=a.Radius+b.Radius){
  return true;
 }else{
  return false;
 }
}

//角錐台
void tpyramid(float lw,float lh,float hw,float hh,float depth)
{
 glBegin(GL_QUADS);
 //上面
 glNormal3f(0.0,0.0,1.0);
 glVertex3f(hw/2,hh/2,depth/2);
 glVertex3f(-lw/2,lh/2,depth/2);
 glVertex3f(-lw/2,-lh/2,depth/2);
 glVertex3f(hw/2,-hh/2,depth/2);
 //右面
 glNormal3f(1.0,0.0,0.0);
 glVertex3f(hw/2,hh/2,depth/2);
 glVertex3f(hw/2,hh/2,-depth/2);
 glVertex3f(hw/2,-hh/2,-depth/2);
 glVertex3f(hw/2,-hh/2,depth/2);
 //左面
 glNormal3f(-1.0,0.0,0.0);
 glVertex3f(-lw/2,lh/2,-depth/2);
 glVertex3f(-lw/2,lh/2,depth/2);
 glVertex3f(-lw/2,-lh/2,depth/2);
 glVertex3f(-lw/2,-lh/2,-depth/2);
 //下面
 glNormal3f(0.0,0.0,-1.0);
 glVertex3f(hw/2,hh/2,-depth/2);
 glVertex3f(-lw/2,lh/2,-depth/2);
 glVertex3f(-lw/2,-lh/2,-depth/2);
 glVertex3f(hw/2,-hh/2,-depth/2);
 //前面
 glNormal3f(0.0,1.0,0.0);
 glVertex3f(hw/2,hh/2,depth/2);
 glVertex3f(-lw/2,lh/2,depth/2);
 glVertex3f(-lw/2,lh/2,-depth/2);
 glVertex3f(hw/2,hh/2,-depth/2);
 //後面
 glNormal3f(0.0,-1.0,0.0);
 glVertex3f(hw/2,-hh/2,depth/2);
 glVertex3f(-lw/2,-lh/2,depth/2);
 glVertex3f(-lw/2,-lh/2,-depth/2);
 glVertex3f(hw/2,-hh/2,-depth/2);
 glEnd();
}

//直方体
void cuboid(float width,float height,float depth)
{
 glBegin(GL_QUADS);
 //前面
 glNormal3f(0.0,0.0,-1.0);
 glVertex3f(width/2,height/2,depth/2);
 glVertex3f(-width/2,height/2,depth/2);
 glVertex3f(-width/2,-height/2,depth/2);
 glVertex3f(width/2,-height/2,depth/2);

 //左面
 glNormal3f(1.0,0.0,0.0);
 glVertex3f(width/2,height/2,depth/2);
 glVertex3f(width/2,height/2,-depth/2);
 glVertex3f(width/2,-height/2,-depth/2);
 glVertex3f(width/2,-height/2,depth/2);

 //右面
 glNormal3f(-1.0,0.0,0.0);
 glVertex3f(-width/2,height/2,-depth/2);
 glVertex3f(-width/2,height/2,depth/2);
 glVertex3f(-width/2,-height/2,depth/2);
 glVertex3f(-width/2,-height/2,-depth/2);

 //後面
 glNormal3f(0.0,0.0,1.0);
 glVertex3f(width/2,height/2,-depth/2);
 glVertex3f(-width/2,height/2,-depth/2);
 glVertex3f(-width/2,-height/2,-depth/2);
 glVertex3f(width/2,-height/2,-depth/2);

 //上面
 glNormal3f(0.0,1.0,0.0);
 glVertex3f(width/2,height/2,depth/2);
 glVertex3f(-width/2,height/2,depth/2);
 glVertex3f(-width/2,height/2,-depth/2);
 glVertex3f(width/2,height/2,-depth/2);

 //下面
 glNormal3f(0.0,-1.0,0.0);
 glVertex3f(width/2,-height/2,depth/2);
 glVertex3f(-width/2,-height/2,depth/2);
 glVertex3f(-width/2,-height/2,-depth/2);
 glVertex3f(width/2,-height/2,-depth/2);
 glEnd();
}

//円柱
void pocket_corner(float radius,float height,float width,float depth,int sides)
{
 double pi = 3.1415;
 //上面
 glNormal3d(0.0, 0.0, 1.0);
 glBegin(GL_QUAD_STRIP);
 glVertex3f(width/2,height/2,depth/2);
 glVertex3f(width/2,-height/2,depth/2);
 for(double i = 0; i <= sides/2; i++) {
  double t = 2*PI/sides * (double)i;
  glVertex3f(radius * cos(t), radius * sin(t) - (height/2), depth/2);
 }
 glVertex3f(-width/2,-height/2,depth/2);
 glVertex3f(-width/2,height/2,depth/2);
 glEnd();

 glBegin(GL_QUADS);
 //右面
 glNormal3f(-1.0,0.0,0.0);
 glVertex3f(-width/2,height/2,-depth/2);
 glVertex3f(-width/2,height/2,depth/2);
 glVertex3f(-width/2,-height/2,depth/2);
 glVertex3f(-width/2,-height/2,-depth/2);

 //左面
 glNormal3f(1.0,0.0,0.0);
 glVertex3f(width/2,height/2,depth/2);
 glVertex3f(width/2,height/2,-depth/2);
 glVertex3f(width/2,-height/2,-depth/2);
 glVertex3f(width/2,-height/2,depth/2);

 //後面
 glNormal3f(0.0,0.0,1.0);
 glVertex3f(width/2,height/2,-depth/2);
 glVertex3f(-width/2,height/2,-depth/2);
 glVertex3f(-width/2,-height/2,-depth/2);
 glVertex3f(width/2,-height/2,-depth/2);
 glEnd();

 //前面
 glBegin(GL_QUAD_STRIP);
 for(double i = 0; i <= sides/2; i++){
  double t = i*2*PI/sides;
  glNormal3f((GLfloat)cos(t), (GLfloat)sin(t), 0.0);
  glVertex3f((GLfloat)(radius*cos(t)), (GLfloat)(radius*sin(t)-(height/2)), -depth/2);
  glVertex3f((GLfloat)(radius*cos(t)), (GLfloat)(radius*sin(t)-(height/2)), depth/2);
 }
 glEnd();

 //下面
 glNormal3d(0.0, 0.0, -1.0);
 glBegin(GL_QUAD_STRIP);
 glVertex3f(width/2,height/2,-depth/2);
 glVertex3f(width/2,-height/2,-depth/2);
 for(double i = 0; i <= sides/2; i++) {
  double t = 2*PI/sides * (double)i;
  glVertex3f(radius * cos(t), radius * sin(t) - (height/2), -depth/2);
 }
 glVertex3f(-width/2,-height/2,-depth/2);
 glVertex3f(-width/2,height/2,-depth/2);
 glEnd();

}

void Ground(void) {
    double ground_max_x = 300.0;
    double ground_max_y = 300.0;
    glMaterialfv(GL_FRONT, GL_DIFFUSE, limelight);  // 地面の色
    glBegin(GL_LINES);
    for(double ly = -ground_max_y ;ly <= ground_max_y; ly+=20.0){
      glVertex3d(-ground_max_x, ly,0);
      glVertex3d(ground_max_x, ly,0);
    }
    for(double lx = -ground_max_x ;lx <= ground_max_x; lx+=20.0){
      glVertex3d(lx, ground_max_y,0);
      glVertex3d(lx, -ground_max_y,0);
    }
    glEnd();
}

void display(void)
{

 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 glViewport(0, 0, WIDTH, HEIGHT);
 glMatrixMode(GL_PROJECTION);
 glLoadIdentity();
 //視野角,アスペクト比(ウィンドウの幅/高さ),描画する範囲(最も近い距離,最も遠い距離)
 gluPerspective(zoom, (double)WIDTH / (double)HEIGHT, 1.0, 1000.0);
 glMatrixMode(GL_MODELVIEW);
 glLoadIdentity();
 //視点の設定
 float x = angle * PI / 180;
 gluLookAt(400*sin(x),-400.0*cos(x),300.0, //カメラの座標 0.0,-400.0,300.0
	   0.0,0.0,0.0, // 注視点の座標
	   0.0,0.0,1.0); // 画面の上方向を指すベクトル

 //ライトの設定
 glLightfv(GL_LIGHT0, GL_POSITION, lightpos);

 glPushMatrix();
 balls[0].Draw();
 glPopMatrix();

 glPushMatrix();
 balls[1].Draw();
 glPopMatrix();

 glPushMatrix();
 balls[2].Draw();
 glPopMatrix();
 
 glPushMatrix();
 balls[3].Draw();
 glPopMatrix();
 
 glPushMatrix();
 balls[4].Draw();
 glPopMatrix();
 
 glPushMatrix();
 balls[5].Draw();
 glPopMatrix();
 
 glPushMatrix();
 balls[6].Draw();
 glPopMatrix();
 
 glPushMatrix();
 balls[7].Draw();
 glPopMatrix();
 
 glPushMatrix();
 balls[8].Draw();
 glPopMatrix();

 glPushMatrix();
 balls[9].Draw();
 glPopMatrix();

 //1ポケット
 glPushMatrix();
 pocket[0].Draw();
 glPopMatrix();

 //2ポケット    
 glPushMatrix();
 pocket[1].Draw();
 glPopMatrix();

 //3ポケット
 glPushMatrix();
 pocket[2].Draw();
 glPopMatrix();

 //4ポケット
 glPushMatrix();
 pocket[3].Draw();
 glPopMatrix();

 //5ポケット
 glPushMatrix();
 pocket[4].Draw();
 glPopMatrix();

 //6ポケット
 glPushMatrix();
 pocket[5].Draw();
 glPopMatrix();

 //クッション
 glPushMatrix();
 glMaterialfv(GL_FRONT, GL_DIFFUSE, limelight);
 glTranslatef(130.0f,0.0f,10.0f);
 tpyramid(8.0f,110.0f,8.0f,120.0f,10.0f);
 glPopMatrix();

 glPushMatrix();
 glMaterialfv(GL_FRONT, GL_DIFFUSE, limelight);
 glTranslatef(-130.0f,0.0f,10.0f);
 glRotatef(180.0f,0.0f,0.0f,1.0f);
 tpyramid(8.0f,110.0f,8.0f,120.0f,10.0f);
 glPopMatrix();

 glPushMatrix();
 glMaterialfv(GL_FRONT, GL_DIFFUSE, limelight);
 glTranslatef(-65.0f,63.5f,10.0f);
 glRotatef(90.0f,0.0f,0.0f,1.0f);
 tpyramid(8.0f,105.0f,8.0f,118.0f,10.0f);
 glPopMatrix();

 glPushMatrix();
 glMaterialfv(GL_FRONT, GL_DIFFUSE, limelight);
 glTranslatef(65.0f,63.5f,10.0f);
 glRotatef(90.0f,0.0f,0.0f,1.0f);
 tpyramid(8.0f,105.0f,8.0f,118.0f,10.0f);
 glPopMatrix();

 glPushMatrix();
 glMaterialfv(GL_FRONT, GL_DIFFUSE, limelight);
 glTranslatef(-65.0f,-63.5f,10.0f);
 glRotatef(270.0f,0.0f,0.0f,1.0f);
 tpyramid(8.0f,105.0f,8.0f,118.0f,10.0f);
 glPopMatrix();

 glPushMatrix();
 glMaterialfv(GL_FRONT, GL_DIFFUSE, limelight);
 glTranslatef(65.0f,-63.5f,10.0f);
 glRotatef(270.0f,0.0f,0.0f,1.0f);
 tpyramid(8.0f,105.0f,8.0f,118.0f,10.0f);
 glPopMatrix();

 //テーブル床
 glPushMatrix();
 glMaterialfv(GL_FRONT, GL_DIFFUSE, limelight);
 cuboid(290.0f,160.0f,-5.0f);
 glPopMatrix();

 //カバーサイド
 glPushMatrix();
 glMaterialfv(GL_FRONT, GL_DIFFUSE, brown);
 glTranslatef(-65.0f,-75.0f,10.0f);
 cuboid(118.0f,15.0f,-10.0f);//
 glPopMatrix();

 glPushMatrix();
 glMaterialfv(GL_FRONT, GL_DIFFUSE, brown);
 glTranslatef(65.0f,-75.0f,10.0f);
 cuboid(118.0f,15.0f,-10.0f);//
 glPopMatrix();

 glPushMatrix();
 glMaterialfv(GL_FRONT, GL_DIFFUSE, brown);
 glTranslatef(-65.0f,75.0f,10.0f);
 cuboid(118.0f,15.0f,-10.0f);//
 glPopMatrix();

 glPushMatrix();
 glMaterialfv(GL_FRONT, GL_DIFFUSE, brown);
 glTranslatef(65.0f,75.0f,10.0f);
 cuboid(118.0f,15.0f,-10.0f);//
 glPopMatrix();

 glPushMatrix();
 glMaterialfv(GL_FRONT, GL_DIFFUSE, brown);
 glTranslatef(139.0f,0.0f,10.0f);
 cuboid(10.0f,120.0f,-10.0f);//
 glPopMatrix();

 glPushMatrix();
 glMaterialfv(GL_FRONT, GL_DIFFUSE, brown);
 glTranslatef(-139.0f,0.0f,10.0f);
 cuboid(10.0f,120.0f,-10.0f);//
 glPopMatrix();

 //ポケットカバー
 glPushMatrix();
 glMaterialfv(GL_FRONT, GL_DIFFUSE, white);
 glTranslatef(0.0f,75.0f,10.0f);
 cuboid(13.0f,15.0f,-10.0f);//
 glPopMatrix();

 glPushMatrix();
 glMaterialfv(GL_FRONT, GL_DIFFUSE, white);
 glTranslatef(0.0f,-75.0f,10.0f);
 cuboid(13.0f,15.0f,-10.0f);//
 glPopMatrix();

/*
 glPushMatrix();
 //xサイドカバー
 glMaterialfv(GL_FRONT, GL_DIFFUSE, white);
 glTranslatef(0.0f,0.0f,40.0f);
 pocket_corner(6.75f,10.0f,13.0f,5.0f,10.0f);
 glPopMatrix();
*/

 //メッシュ(地面)
 Ground();

 glutSwapBuffers();
}


void Hitbox(void){//当たり判定による衝突後の座標計算

    float v0, v1, v2, dir1, dir2, dir, diffx, diffy;
    float ux, uy, vx, vy;
    float udx, udy, vdx, vdy;

    for(int i=0; i<=8; i++){
	for(int j=i+1; j<=9; j++){ 
	    if(Calc_Hit(balls[i],balls[j])){
		//自球側から相手球のベクトル計算
		v0 = sqrt((balls[i].Speed.x*balls[i].Speed.x)+(balls[i].Speed.y*balls[i].Speed.y));
		diffx = balls[j].Pos.x-balls[i].Pos.x;//球間のx距離
		diffy = balls[j].Pos.y-balls[i].Pos.y;//球間のy距離
		dir1 = atan2f(diffy,diffx);//自球と相手球の角度
		dir2 = atan2f(balls[i].Speed.y, balls[i].Speed.x);//自球進行方向
                dir = dir2 - dir1;//角度差
                v1 = fabs(v0*cos(dir));//衝突後相手球の速度
                v2 = fabs(v0*sin(dir));//衝突後自球の速度
                vx = v1*cos(dir1);//相手球x速度
                vy = v1*sin(dir1);//相手球y速度

		if(sin(dir)<0){
                    ux = v2*cos(dir1-PI/2);//衝突後自球x速度
                    uy = v2*sin(dir1-PI/2);//衝突後自球y速度
		}else{
		    ux = v2*cos(dir1+PI/2);
		    uy = v2*sin(dir1+PI/2);
		}
		//相手球側から自球のベクトル計算
		v0 = sqrt((balls[j].Speed.x*balls[j].Speed.x)+(balls[j].Speed.y*balls[j].Speed.y));
                diffx = balls[i].Pos.x-balls[j].Pos.x;
		diffy = balls[i].Pos.y-balls[j].Pos.y;
                dir1 = atan2f(diffy,diffx);
		dir2 = atan2f(balls[j].Speed.y, balls[j].Speed.x);
		dir = dir2 - dir1;
		v1 = fabs(v0*cos(dir));
		v2 = fabs(v0*sin(dir));
		vdx = v1*cos(dir1);
		vdy = v1*sin(dir1);

		if(sin(dir)<0){
		    udx = v2*cos(dir1-PI/2);
		    udy = v2*sin(dir1-PI/2);
		}else{
		    udx = v2*cos(dir1+PI/2);
		    udy = v2*sin(dir1+PI/2);
		}
                //衝突による速度合成
		balls[i].Speed.x = 0.5*(1+ball_e)*(ux+vdx);
		balls[i].Speed.y = 0.5*(1+ball_e)*(uy+vdy);
		balls[j].Speed.x = 0.5*(1+ball_e)*(vx+udx);
		balls[j].Speed.y = 0.5*(1+ball_e)*(vy+udy);
	    }
	}
    }

    //壁の反射や床の摩擦による速度減衰
    for(int i=0; i<=9; i++){
	    balls[i].Pos.x += balls[i].Speed.x;
	    balls[i].Pos.y += balls[i].Speed.y;
	    balls[i].Speed.x *= floor_e;
	    balls[i].Speed.y *= floor_e;
	    if(balls[i].Pos.y>=-55.0f && balls[i].Pos.y<=55.0f){ 
		if((balls[i].Pos.x+balls[i].Radius)>127.0f){
		    balls[i].Speed.x*=-1.0f*wall_e;
		    balls[i].Speed.y*=wall_e;
		    balls[i].Pos.x-=balls[i].Pos.x-127.0+balls[i].Radius;
		}else if((balls[i].Pos.x-balls[i].Radius)<-127.0f){
		    balls[i].Speed.x*=-1.0f*wall_e;
		    balls[i].Speed.y*=wall_e;
		    balls[i].Pos.x+=-1.0*(balls[i].Pos.x)-127.0+balls[i].Radius;
		}
	    }
	    if(balls[i].Pos.x>=-117.5f && balls[i].Pos.x<=-12.5f){
		if((balls[i].Pos.y+balls[i].Radius)>63.5){
		    balls[i].Speed.y*=-1.0f*wall_e;
		    balls[i].Speed.x*=wall_e;
		    if(balls[i].flag==false) balls[i].Pos.y-=balls[i].Pos.y-63.5+balls[i].Radius;
		}else if((balls[i].Pos.y-balls[i].Radius)<-63.5f){
		    balls[i].Speed.y*=-1.0f*wall_e;
		    balls[i].Speed.x*=wall_e;
		    if(balls[i].flag==false) balls[i].Pos.y+=-1.0*(balls[i].Pos.y)-63.5+balls[i].Radius;
		}
	    }
	    if(balls[i].Pos.x>=12.5f && balls[i].Pos.x<=117.5f){
		if((balls[i].Pos.y+balls[i].Radius)>63.5f){
		    balls[i].Speed.y*=-1.0f*wall_e;
		    balls[i].Speed.x*=wall_e;
		    if(balls[i].flag==false) balls[i].Pos.y-=balls[i].Pos.y-63.5+balls[i].Radius;
		}else if((balls[i].Pos.y-balls[i].Radius)<-63.5f){
		    balls[i].Speed.y*=-1.0f*wall_e;
		    balls[i].Speed.x*=wall_e;
		    if(balls[i].flag==false) balls[i].Pos.y+=-1.0*(balls[i].Pos.y)-63.5+balls[i].Radius;
	    }
	    }
    }
}

void Erasebox(void){//当たり判定によるビリヤード球の消去
    for(int i=0; i<=9; i++){
	for(int j=0; j<=5; j++){
	    if(Calc_Erase(balls[i], pocket[j])){
		balls[i].flag=true;
		balls[i].Pos.x=0.0f+11.0f*i;
		balls[i].Pos.y=-100.0f;
		balls[i].Speed.x=0.0f;
		balls[i].Speed.y=0.0f;
	    }
	}
    }
}

void idle(void)
{
 if(flag){
 angle+=0.2f;
 if(angle>360.0f) angle-=360.0f;
 }
 Hitbox();
 Erasebox();
 glutPostRedisplay();
}




void Init(){
 glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
 glEnable(GL_DEPTH_TEST);
 glEnable(GL_LIGHTING);
 glEnable(GL_LIGHT0);

  //自球0番ボール
 balls[0].Color[0]=white[0];
 balls[0].Color[1]=white[1];
 balls[0].Color[2]=white[2];
 balls[0].Color[3]=white[3];
 balls[0].Pos.x=-65.0f;
 balls[0].Pos.y=0.0f;
 balls[0].Pos.z=5.0f;
 balls[0].Radius=5.0f;
 balls[0].Speed.x=0.0f;
 balls[0].Speed.y=0.0f;
 balls[0].flag=false;

  //1番ボール
 balls[1].Color[0]=yellow[0];
 balls[1].Color[1]=yellow[1];
 balls[1].Color[2]=yellow[2];
 balls[1].Color[3]=yellow[3];
 balls[1].Pos.x=50.0f;
 balls[1].Pos.y=0.0f;
 balls[1].Pos.z=5.0f;
 balls[1].Radius=5.0f;
 balls[1].flag=false;

  //2番ボール
 balls[2].Color[0]=blue[0];
 balls[2].Color[1]=blue[1];
 balls[2].Color[2]=blue[2];
 balls[2].Color[3]=blue[3];
 balls[2].Pos.x=70.0f;
 balls[2].Pos.y=10.0f;
 balls[2].Pos.z=5.0f;
 balls[2].Radius=5.0f;
 balls[2].flag=false;

  //3番ボール
 balls[3].Color[0]=red[0];
 balls[3].Color[1]=red[1];
 balls[3].Color[2]=red[2];
 balls[3].Color[3]=red[3];
 balls[3].Pos.x=90.0f;
 balls[3].Pos.y=0.0f;
 balls[3].Pos.z=5.0f;
 balls[3].Radius=5.0f;
 balls[3].flag=false;

  //4番ボール
 balls[4].Color[0]=purple[0];
 balls[4].Color[1]=purple[1];
 balls[4].Color[2]=purple[2];
 balls[4].Color[3]=purple[3];
 balls[4].Pos.x=70.0f;
 balls[4].Pos.y=-10.0f;
 balls[4].Pos.z=5.0f;
 balls[4].Radius=5.0f;
 balls[4].flag=false;

  //5番ボール
 balls[5].Color[0]=orange[0];
 balls[5].Color[1]=orange[1];
 balls[5].Color[2]=orange[2];
 balls[5].Color[3]=orange[3];
 balls[5].Pos.x=60.0f;
 balls[5].Pos.y=5.0f;
 balls[5].Pos.z=5.0f;
 balls[5].Radius=5.0f;
 balls[5].flag=false;

  //6番ボール
 balls[6].Color[0]=green[0];
 balls[6].Color[1]=green[1];
 balls[6].Color[2]=green[2];
 balls[6].Color[3]=green[3];
 balls[6].Pos.x=60.0f;
 balls[6].Pos.y=-5.0f;
 balls[6].Pos.z=5.0f;
 balls[6].Radius=5.0f;
 balls[6].flag=false;

  //7番ボール
 balls[7].Color[0]=brown[0];
 balls[7].Color[1]=brown[1];
 balls[7].Color[2]=brown[2];
 balls[7].Color[3]=brown[3];
 balls[7].Pos.x=80.0f;
 balls[7].Pos.y=5.0f;
 balls[7].Pos.z=5.0f;
 balls[7].Radius=5.0f;
 balls[7].flag=false;

  //8番ボール
 balls[8].Color[0]=black[0];
 balls[8].Color[1]=black[1];
 balls[8].Color[2]=black[2];
 balls[8].Color[3]=black[3];
 balls[8].Pos.x=80.0f;
 balls[8].Pos.y=-5.0f;
 balls[8].Pos.z=5.0f;
 balls[8].Radius=5.0f;
 balls[8].flag=false;

  //9番ボール
 balls[9].Color[0]=lyellow[0];
 balls[9].Color[1]=lyellow[1];
 balls[9].Color[2]=lyellow[2];
 balls[9].Color[3]=lyellow[3];
 balls[9].Pos.x=70.0f;
 balls[9].Pos.y=0.0f;
 balls[9].Pos.z=5.0f;
 balls[9].Radius=5.0f;
 balls[9].flag=false;

 //1番ポケット
 pocket[0].Color[0]=black[0];
 pocket[0].Color[1]=black[1];
 pocket[0].Color[2]=black[2];
 pocket[0].Color[3]=black[3];
 pocket[0].Pos.x=130.0f;
 pocket[0].Pos.y=64.0f;
 pocket[0].Pos.z=0.0f;
 pocket[0].Radius=6.75f;

 //2番ポケット
 pocket[1].Color[0]=black[0];
 pocket[1].Color[1]=black[1];
 pocket[1].Color[2]=black[2];
 pocket[1].Color[3]=black[3];
 pocket[1].Pos.x=130.0f;
 pocket[1].Pos.y=-64.0f;
 pocket[1].Pos.z=0.0f;
 pocket[1].Radius=6.75f;

 //3番ポケット
 pocket[2].Color[0]=black[0];
 pocket[2].Color[1]=black[1];
 pocket[2].Color[2]=black[2];
 pocket[2].Color[3]=black[3];
 pocket[2].Pos.x=0.0f;
 pocket[2].Pos.y=65.0f;
 pocket[2].Pos.z=0.0f;
 pocket[2].Radius=6.75f;

 //4番ポケット
 pocket[3].Color[0]=black[0];
 pocket[3].Color[1]=black[1];
 pocket[3].Color[2]=black[2];
 pocket[3].Color[3]=black[3];
 pocket[3].Pos.x=0.0f;
 pocket[3].Pos.y=-65.0f;
 pocket[3].Pos.z=0.0f;
 pocket[3].Radius=6.75f;

 //5番ポケット
 pocket[4].Color[0]=black[0];
 pocket[4].Color[1]=black[1];
 pocket[4].Color[2]=black[2];
 pocket[4].Color[3]=black[3];
 pocket[4].Pos.x=-130.0f;
 pocket[4].Pos.y=64.0f;
 pocket[4].Pos.z=0.0f;
 pocket[4].Radius=6.75f;

 //6番ポケット
 pocket[5].Color[0]=black[0];
 pocket[5].Color[1]=black[1];
 pocket[5].Color[2]=black[2];
 pocket[5].Color[3]=black[3];
 pocket[5].Pos.x=-130.0f;
 pocket[5].Pos.y=-64.0f;
 pocket[5].Pos.z=0.0f;
 pocket[5].Radius=6.75f;
}

void command(unsigned char key, int x, int y){
    float dir = roll * PI /180;
    switch(key){
    //space key
    case ' '://自球の射出
	balls[0].Speed.x=10.0f*cos(dir);
	balls[0].Speed.y=10.0f*sin(dir);
	break;
    case 'w'://自球を初期位置へリセット
	balls[0].Pos.x=-65.0f;
	balls[0].Pos.y=0.0f;
	break;
    case 's'://カメラの方向移動(円形)
	flag = true;
	break;
    case 'd'://カメラの移動停止
	flag = false;
	break;
    case 'r'://全球の初期位置リセット
    for(int i=0; i<=9; i++){
	balls[i].flag=false;
	balls[i].Speed.x=0.0f;
	balls[i].Speed.y=0.0f;
    }
	Init();
	break;
    case 'q'://終了
	exit(0);
	break;
    default:
	break;
    }
}

void action(int key, int x, int y){
    switch(key){
    case GLUT_KEY_UP://上キーでズームアウト
	zoom += 0.2f;
	break;
    case GLUT_KEY_DOWN://下キーでズームアップ
	zoom -= 0.2f;
	break;
    case GLUT_KEY_LEFT://左回りに射出方向の決定
	roll += 1.0f;
	break;
    case GLUT_KEY_RIGHT://右回りに射出方向の決定
	roll -= 1.0f;
	break;
    default:
	break;
    }
}

int main(int argc, char *argv[])
{
 glutInitWindowPosition(200, 200);
 glutInitWindowSize(WIDTH, HEIGHT);
 glutInit(&argc, argv);
 glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
 glutCreateWindow("billiards");
 glutDisplayFunc(display);
 glutKeyboardFunc(command);
 glutSpecialFunc(action);
 glutIdleFunc(idle);
 Init();
 glutMainLoop();
 return 0;
}
