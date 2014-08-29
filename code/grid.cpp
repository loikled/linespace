#include "grid.h"


const float Grid::defaultUnit = 0.05f;
Grid::Grid(QObject *parent) :
    QObject(parent),
    unit_(defaultUnit),
    left_(Leap::Vector(-1.0,-1.0,0.5)),
    right_(Leap::Vector(1.0, -1.0, 0.5)),
    fill_(false)
{
    computeLines();
}

const Grid::GridList_t* Grid::getLineList() const{
    return &grid_;
}

//compute the coordinates of the lines of the grid,
//to be called only when properties change
void Grid::computeLines(){
    grid_.clear();

    if (!fill_){
        float total = right_.distanceTo(left_);
        //back XY plane
        DrawPlaneXY(left_.z - total);
        //floor plane
        DrawPlaneXZ(left_.y);
        //ceiling plane
        DrawPlaneXZ(left_.y + total);
        //left wall plane
        DrawPlaneYZ(left_.x);
        //right wall plane
        DrawPlaneYZ(right_.x);
    }
}

//draws a plane starting from left to right coordinates at given z
void Grid::DrawPlaneXY(float z){
    float total = right_.distanceTo(left_);

    //vertical lines
    for(float x = left_.x; x < right_.x; x += unit_){
        GridLine_t line;
        line.first = Leap::Vector(x, left_.y , z);
        line.second = Leap::Vector(x,left_.y + total, z);
        grid_.push_back(line);
    }
    //horizontal lines
    for(float y = left_.y; y < left_.y + total; y += unit_){
        GridLine_t line;
        line.first = Leap::Vector(left_.x, y, z);
        line.second = Leap::Vector(right_.x , y, z);
        grid_.push_back(line);
    }
}

void Grid::DrawPlaneXZ(float y){
    float total = right_.distanceTo(left_);

    //horizontal lines going in the deeep
    for(float x = left_.x; x < right_.x; x += unit_){
        GridLine_t line;
        line.first = Leap::Vector(x, y, left_.z);
        line.second = Leap::Vector(x, y, left_.z - total);
        grid_.push_back(line);
    }
    //horizontal lines from near to far
    for(float z = left_.z; z > left_.z - total; z -= unit_){
        GridLine_t line;
        line.first = Leap::Vector(left_.x, y, z);
        line.second = Leap::Vector(left_.x + total, y, z);
        grid_.push_back(line);
    }
}

void Grid::DrawPlaneYZ(float x){
    float total = right_.distanceTo(left_);

    //horizontal line going deep in the screen
    for(float y = left_.y; y < left_.y + total; y += unit_){
        GridLine_t line;
        line.first = Leap::Vector(x, y, left_.z);
        line.second = Leap::Vector(x, y, left_.z - total); //reverted axis
        grid_.push_back(line);
    }
    //vertical lines from near to far
    for(float z = left_.z; z > left_.z - total; z -= unit_){
        GridLine_t line;
        line.first = Leap::Vector(x, left_.y , z);
        line.second = Leap::Vector(x,left_.y + total, z);
        grid_.push_back(line);
    }
}

void Grid::setUnit(float unit){
    if (unit > 0 && unit <= 1.0f){
        unit_ = unit;
    }else{
        unit_ = defaultUnit;
    }
}

void Grid::setBorders(Leap::Vector& left, Leap::Vector& right){
    left_ = left;
    right_ = right;
    computeLines();
}

//display internal grid or only walls (cage)
void Grid::enableFill(bool fill){
    fill_ = fill;
    computeLines();//need to recompute
}
