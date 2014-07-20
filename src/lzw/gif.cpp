#include "gif.h"
#include "frame.h"
#include <iostream>

using namespace std;


unsigned char *Gif::getGCT() const
{
    return m_GCT;
}

void Gif::setGCT(unsigned char *p_value, int p_size)
{
    if(m_ownGCT == 1) delete[] m_GCT;
    m_sizeOfGCT = p_size;
    m_GCT = p_value;
    m_ownGCT = 0;
}

void Gif::setGCT(vector<unsigned char> p_value, int p_size)
{
    m_sizeOfGCT = p_size;
    if(m_ownGCT == 1) delete[] m_GCT;
    m_GCT = new unsigned char[p_size];
    std::copy(p_value.begin(), p_value.end(), m_GCT);
    m_ownGCT = 1;
}

int Gif::getGctFlag() const
{
    return m_gctFlag;
}

void Gif::setGctFlag(int p_value)
{
    m_gctFlag = p_value;
}

int Gif::getSizeOfGCT() const
{
    return m_sizeOfGCT;
}

void Gif::setSizeOfGCT(int p_value)
{
    m_sizeOfGCT = p_value;
}

int Gif::getBgColor() const
{
    return m_bgColor;
}

void Gif::setBgColor(int p_value)
{
    m_bgColor = p_value;
}

int Gif::getSizeOfFrames() const
{
    return m_sizeOfFrames;
}

void Gif::extendFrames()
{
    Frame *f = new Frame();
	m_frames.push_back(f);
    m_sizeOfFrames++;
}

Gif &Gif::operator=(const Gif &p_toCopy)
{
    if(&p_toCopy != this){
        m_gctFlag = p_toCopy.m_gctFlag;
        m_sizeOfGCT = p_toCopy.m_sizeOfGCT;
        m_bgColor = p_toCopy.m_bgColor;
        m_sizeOfFrames = p_toCopy.m_sizeOfFrames;
        height = p_toCopy.height;
        width = p_toCopy.width;
		pixel = NULL;
        delete[] m_GCT;
        m_GCT = new unsigned char[m_sizeOfGCT*3];
		for (int i = 0; i < m_sizeOfGCT*3; i++) {
            m_GCT[i] = p_toCopy.m_GCT[i];
        }
        m_ownGCT = 1;

		for (int i = (int)(m_frames.size())-1; i > 0; --i) {
			delete m_frames[i];
			m_frames.pop_back();
		}

		for (size_t i = 0; i < p_toCopy.m_frames.size(); ++i) {
			m_frames.push_back(p_toCopy.m_frames[i]->clone());
		}
    }
    return *this;
}

Gif::Gif(){
    m_sizeOfGCT = 0;
    m_GCT = NULL;
    m_sizeOfFrames = 0;
    m_bgColor = 0;
    m_gctFlag = 0;
    m_ownGCT = 0;
}

Gif::Gif(const Gif& p_toCopy) : Picture(){
	m_gctFlag = p_toCopy.m_gctFlag;
	m_sizeOfGCT = p_toCopy.m_sizeOfGCT;
	m_bgColor = p_toCopy.m_bgColor;
	m_sizeOfFrames = p_toCopy.m_sizeOfFrames;
	height = p_toCopy.height;
	width = p_toCopy.width;
	m_ownGCT = p_toCopy.m_ownGCT;
	pixel = NULL;
	m_GCT = new unsigned char[m_sizeOfGCT*3];
	for (int i = 0; i < m_sizeOfGCT*3; i++) {
		m_GCT[i] = p_toCopy.m_GCT[i];
	}
	for (size_t i = 0; i < p_toCopy.m_frames.size(); ++i) {
		m_frames.push_back(p_toCopy.m_frames[i]->clone());
	}
}

Gif::Gif(const Gif& p_toCopy) : Picture(){
	m_gctFlag = p_toCopy.m_gctFlag;
	m_sizeOfGCT = p_toCopy.m_sizeOfGCT;
	m_bgColor = p_toCopy.m_bgColor;
	m_sizeOfFrames = p_toCopy.m_sizeOfFrames;
	height = p_toCopy.height;
	width = p_toCopy.width;
	pixel = NULL;
	m_colorTable = new char[m_sizeOfGCT*3];
	for (int i = 0; i < m_sizeOfGCT*3; i++) {
		m_colorTable[i] = p_toCopy.m_colorTable[i];
	}
	m_frames = p_toCopy.m_frames;
}

Gif::~Gif()
{
    if(m_ownGCT == 1) delete[] m_GCT;
}


Frame *Gif::getFrame(int frame)
{
	return m_frames.at(frame);
}
