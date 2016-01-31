#ifndef PRIMITIVES_H_
#define PRIMITIVES_H_

#ifndef GLOBAL_H_
#include "Global.h"
#endif
#include <algorithm>

#ifdef __ANDROID__
#define KEY_ESC   AKEYCODE_BACK
#define KEY_ENTER 0
#define KEY_LEFT  AKEYCODE_DPAD_LEFT
#define KEY_RIGHT AKEYCODE_DPAD_RIGHT
#else
#include <GL/freeglut.h>
#define KEY_ESC   27
#define KEY_ENTER 13
#define KEY_LEFT  GLUT_KEY_LEFT
#define KEY_RIGHT GLUT_KEY_RIGHT
#endif

template<class T> int sign(T v){
  if (v>0) return 1;
  if (v<0) return -1;
  else return 0;
}

template<class T> class Vector2;
template<class T> class Segment;
template<class T> class Rect;
template<class T> float getDist(const Vector2<T>& p1, const Vector2<T>& p2);
template<class T> float getDist(const Vector2<T>& p1, const Segment<T>& s1);
template<class T> float getDist(const Segment<T>& s1, const Segment<T>& s2);
template<class T> bool IsIntersect(const Segment<T>& s1, const Segment<T>& s2);
template<class T> bool IsIntersect(const Segment<T>& s, const Rect<T>& r);
template<class T> bool IsIntersect(const Rect<T>& r, const Segment<T>& s);
template<class T> bool IsIntersect(const Rect<T>& r1, const Rect<T>& r2);

template<class T>
class Vector2{
  T x_,y_;
public:
  T x() const;
  T y() const;
  void set(Vector2<T> v);
  void set(T x, T y);
  void setX(T x);
  void setY(T y);
  Vector2(T x = 0, T y = 0);
  //Vector2(const Vector2<int>& vi):x_(vi.x()),y_(vi.y()){}
  template <class T1>
  operator Vector2<T1>()
  {
      return Vector2<T1>((T1)x_,(T1)y_);
  }
};

template<class T>
Vector2<T> operator - (const Vector2<T>& l, const Vector2<T>& r);

template<class T>
class Segment{
  Vector2<T> p1_, p2_;
public:
  const Vector2<T>& p1()const;
  const Vector2<T>& p2()const;
  T x1()const;
  T y1()const;
  T x2()const;
  T y2()const;
  Segment(T x1=0, T y1=0, T x2=0, T y2=0);
  Segment(const Vector2<T>& p1, const Vector2<T>& p2);
  template<class T1>
  operator Segment<T1>()
  {
      return Segment<T1>((Vector2<T1>)p1_,(Vector2<T1>)p2_);
  }
  T getWidth()const;
  void set(T x1,T y1,T x2, T y2);
  void setY(T y);
};
template<class T>
bool cmpY(Segment<T>& s1, Segment<T>& s2);

template<class T>
class Rect{
  Vector2<T> pos_;
  Vector2<T> size_;
public:
  Rect( T x=0, T y=0, T w=0, T h=0 );
  const Vector2<T>& getPos()const;
  const Vector2<T>& getSize()const;
  T x()const;
  T y()const;
  T getWidth()const;
  T getHeight()const;
  void setPos(const Vector2<T> &pos);
  void setSize(const Vector2<T> &size);
  void set(T x, T y, T w, T h){pos_.set(x,y);size_.set(w,h);}
  void setX(T x);
  void setY(T y);
  void setWidth(T w);
  void setHeight(T h);
  T getTop()const;
  T getBottom()const;
  T getLeft()const;
  T getRight()const;
  Vector2<T> getRightTop();
  Vector2<T> getLeftBottom();
  Vector2<T> getRightBottom();
  bool isContain(const Vector2<T> &point )const;
};

// implementation
template<class T> T Vector2<T>::x() const{ return x_; }
template<class T> T Vector2<T>::y() const{ return y_; }
template<class T> Vector2<T>::Vector2(T x, T y) : x_(x), y_(y){}
template<class T> Vector2<T> operator - (const Vector2<T>& l, const Vector2<T>& r){
  return Vector2<T>(l.x()-r.x(),l.y()-r.y());
}
template<class T> void Vector2<T>::set(T x, T y){ x_ = x; y_ = y; }
template<class T> void Vector2<T>::set(Vector2<T> v){ set(v.x(),v.y()); }
template<class T> void Vector2<T>::setX(T x){ x_ = x; }
template<class T> void Vector2<T>::setY(T y){ y_ = y; }

template<class T> Segment<T>::Segment(T x1, T y1, T x2, T y2):p1_(x1,y1),p2_(x2,y2){}
template<class T> Segment<T>::Segment(const Vector2<T>& p1, const Vector2<T>& p2):p1_(p1),p2_(p2){}
template<class T> T Segment<T>::getWidth()const{return (T)getDist(p1_,p2_);}
template<class T> const Vector2<T>& Segment<T>::p1()const{return p1_;}
template<class T> const Vector2<T>& Segment<T>::p2()const{return p2_;}
template<class T> T Segment<T>::x1()const{return p1().x();}
template<class T> T Segment<T>::y1()const{return p1().y();}
template<class T> T Segment<T>::x2()const{return p2().x();}
template<class T> T Segment<T>::y2()const{return p2().y();}
template<class T> void Segment<T>::set(T x1,T y1,T x2, T y2){ p1_.set(x1,y1); p2_.set(x2,y2); }
template<class T> void Segment<T>::setY(T y){ p1_.setY(y); p2_.setY(y); }

template<class T> Rect<T>::Rect( T x, T y, T w, T h):pos_(x, y), size_(w, h) { }
template<class T> const Vector2<T>& Rect<T>::getPos()const { return pos_; }
template<class T> const Vector2<T>& Rect<T>::getSize()const { return size_; }
template<class T> void Rect<T>::setPos(const Vector2<T> &pos){ pos_ = pos; }
template<class T> void Rect<T>::setSize(const Vector2<T> &size){ size_ = size; }
template<class T> void Rect<T>::setX(T x){pos_.setX(x);}
template<class T> void Rect<T>::setY(T y){pos_.setY(y);}
template<class T> void Rect<T>::setWidth(T w){size_.setX(w);}
template<class T> void Rect<T>::setHeight(T h){size_.setY(h);}
template<class T> T Rect<T>::x()const{return pos_.x();}
template<class T> T Rect<T>::y()const{return pos_.y();}
template<class T> T Rect<T>::getWidth()const{return size_.x();}
template<class T> T Rect<T>::getHeight()const{return size_.y();}
template<class T> T Rect<T>::getTop()const{ return pos_.y(); }
template<class T> T Rect<T>::getBottom()const{ return pos_.y()+size_.y(); }
template<class T> T Rect<T>::getLeft()const{ return pos_.x(); }
template<class T> T Rect<T>::getRight()const{ return pos_.x()+size_.x(); }

template<class T> bool Rect<T>::isContain(const Vector2<T> &p )const{
  bool r=getLeft() <= p.x() && getRight()  >= p.x() && 
         getTop()  <= p.y() && getBottom() >= p.y();
  return r;
}

template<class T> bool cmpY(const Segment<T>& s1, const Segment<T>& s2){ return s1.p1().y() < s2.p1().y(); }

template<class T> float getDistSqr(const Vector2<T>& p1, const Vector2<T>& p2){
  return static_cast<float>( (p1.x()-p2.x())*(p1.x()-p2.x())+(p1.y()-p2.y())*(p1.y()-p2.y()) );
}

template<class T> float getDist(const Vector2<T>& p1, const Vector2<T>& p2){
  return sqrt(getDistSqr(p1,p2));
}

template<class T> float getDist(const Vector2<T>& p1, const Segment<T>& s1){
  float a = getDistSqr(p1, s1.p1());
  float b = getDistSqr(p1, s1.p2());
  float c = static_cast<float>( s1.getWidth()*s1.getWidth() );
  if(a>=b+c) return sqrt(b);
  if(b>=a+c) return sqrt(a);
  a=sqrt(a); b=sqrt(b); c=sqrt(c);
  float p = (a+b+c)/2.0f;
  float s = sqrt(p*(p-a)*(p-b)*(p-c));
  float h = s*2.0f/c;
  return h;

  //float A=s1.y1()-s1.y2();
  //float B=s1.x2()-s1.x1();
  //float C=s1.x1()*s1.y2()-s1.x2()*s1.y1();
  //float n = (A*p1.x()+B*p1.y()+C);
  //float d = (float)(A*p1.x()+B*p1.y()+C)/sqrt(A*A+B*B);
  //return d;
}

template<class T> float getDist(const Segment<T>& s1, const Segment<T>& s2){
  float d1 = getDist(s1.p1(),s2);
  float d2 = getDist(s1.p2(),s2);
  float d3 = getDist(s2.p1(),s1);
  float d4 = getDist(s2.p2(),s1);
  return std::min( std::min(d1,d2), std::min(d3,d4) );
}

//http://e-maxx.ru/algo/segments_intersection_checking
template<class T> int area (const Vector2<T>& a, const Vector2<T>& b, const Vector2<T>& c) {
	return (int)(b.x() - a.x()) * (c.y() - a.y()) - (b.y() - a.y()) * (c.x() - a.x());
}
inline bool intersect_1 (int a, int b, int c, int d) {
	if (a > b)  std::swap (a, b);
	if (c > d)  std::swap (c, d);
	return std::max(a,c) <= std::min(b,d);
} 
template<class T> bool intersect (const Vector2<T>& a, const Vector2<T>& b, const Vector2<T>& c, const Vector2<T>& d) {
	return intersect_1 (a.x(), b.x(), c.x(), d.x())
		&& intersect_1 (a.y(), b.y(), c.y(), d.y())
		&& area(a,b,c) * area(a,b,d) <= 0
		&& area(c,d,a) * area(c,d,b) <= 0;
}

template<class T> bool IsIntersect(const Segment<T>& s1, const Segment<T>& s2){
  return intersect(s1.p1(),s1.p2(),s2.p1(),s2.p2());
}
template<class T> bool IsIntersect(const Segment<T>& s, const Rect<T>& r){
  Segment<T> s1(r.getRight(),r.getTop(),r.getRight(),r.getBottom());
  Segment<T> s2(r.getLeft(),r.getTop(),r.getLeft(),r.getBottom());
  return IsIntersect(s, s1) || IsIntersect(s, s2);
}
template<class T> bool IsIntersect(const Rect<T>& r, const Segment<T>& s){
  return IsIntersect(s,r);
}
template<class T> bool IsIntersect(const Rect<T>& r1, const Rect<T>& r2){
  return true;// todo
}

//class LineEquation{
//  float A,B,C;
//public:
//  LineEquation(Segment<float> s){
//    A=s.y1()-s.y2();
//    B=s.x2()-s.x1();
//    C=s.x1()*s.y2()-s.x2()*s.y1();
//  }
//};

#endif
