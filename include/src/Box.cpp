#include "BananaSDK/Box.h"
#include "BananaSDK/Button.h"
#include "BananaSDK/UI.h"
Box::Box(float x, float y, float w, float h) : m_X(x), m_Y(y), m_W(w), m_H(h) {}
void Box::SetColor(float r, float g, float b, float a) { m_R=r; m_G=g; m_B=b; m_A=a; }
void Box::SetRadius(float r)       { m_Radius=r; }
void Box::SetPosition(float x, float y) { m_X=x; m_Y=y; }
void Box::SetSize(float w, float h)     { m_W=w; m_H=h; }
void Box::Draw(UIRenderer& ui) { ui.DrawRect(m_X, m_Y, m_W, m_H, m_R, m_G, m_B, m_A, m_Radius); }
