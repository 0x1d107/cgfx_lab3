#include <vector>
#include <exception>
#include <cstdlib>
#include <SFML/Graphics.hpp>
#include <cmath>

class Matrix{
    struct MatrixSize{
        int w;
        int h;
    } size;
    std::vector<double> vec;
    public:
    Matrix(int h, int w, const double *coefficients){
        size.w = w;
        size.h = h;
        vec.assign(coefficients,coefficients+w*h);
    }
    Matrix(int h, int w, std::initializer_list<double> coefficients){
        size.w = w;
        size.h = h;
        vec.assign(coefficients.begin(),coefficients.end());
    }
    static Matrix v2m3x1(sf::Vector2f vec){
        double m[]={vec.x,vec.y,1};
        return Matrix(3,1,m);
    }
    double get(int i,int j) const{
        return vec[size.w*i+j];
    }
    sf::Vector2f toVec() const{
        return sf::Vector2f{(float)get(0,0),(float)get(1,0)};
    }
    Matrix operator*(const Matrix& a) const{
        const Matrix &b = *this;
        if(a.size.w != b.size.h)
            throw std::runtime_error("Can't multiply matricies with incompatible sizes!");
        int h = a.size.h;
        int w = b.size.w;
        std::vector<double> v;
        for(int i =0;i< h;i++)
            for(int j =0;j<w;j++){
                double sum = 0;
                for(int k =0;k<a.size.w;k++)
                    sum+= a.get(i,k) * b.get(k,j);
                v.push_back(sum);
            }
        return Matrix(h,w,v.data());
    }
    static Matrix translation(double x,double y){

        return Matrix(3,3,{1,0,x,
                           0,1,y,
                           0,0,1});
    }
    static Matrix rotationAroundOrigin(double phi){
        return Matrix(3,3,{cos(phi), sin(phi),0,
                           -sin(phi),  cos(phi),0,
                           0,         0,       1});
    }
    static Matrix scaleOrigin(double x,double y){
        return Matrix(3,3,{x, 0, 0,
                           0, y, 0,
                           0, 0, 1});
    }
    static Matrix shearHOrigin(double m){
        return Matrix(3,3,{1, m, 0,
                           0, 1, 0,
                           0, 0, 1});
    }
    static Matrix shearVOrigin(double m){
        return Matrix(3,3,{1, 0, 0,
                           m, 1, 0,
                           0, 0, 1});
    }

};
class PolygonShape{
    
    public:
    sf::VertexArray va;
    PolygonShape(int n){
        srand(time(NULL));
        va.setPrimitiveType(sf::PrimitiveType::LineStrip);
        va.clear();
        for(int i=0;i< n;i++){
            va.append(sf::Vertex(sf::Vector2f{(float)(rand()%800),(float)(rand()%600)},sf::Color::White));
        }
        va.append(va[0]);
    }
    PolygonShape(const PolygonShape&p ){
        va.setPrimitiveType(sf::LineStrip);
        va.clear();
        for(int i =0;i<p.va.getVertexCount();i++){
            va.append(p.va[i]);
        }
    }
    void transform(const Matrix& a){
        for(int i = 0;i<va.getVertexCount();i++){
            sf::Vertex &v = va[i];
            
            v.position = (Matrix::v2m3x1(v.position)*a).toVec();
        }
    }
    void translate(double x,double y){
        transform(Matrix::translation(x,y));
    }
    void rotateAroundOrigin(double phi){
        transform(Matrix::rotationAroundOrigin(phi));
    }
    void rotateAroundPoint(double x,double y,double phi){
        translate(-x,-y);
        rotateAroundOrigin(phi);
        translate(x,y);
    }
    void scaleAroundOrigin(double kx,double ky){
        transform(Matrix::scaleOrigin(kx,ky));
    }
    void scaleAroundPoint(double x,double y,double kx,double ky){
        translate(-x,-y);
        scaleAroundOrigin(kx,ky);
        translate(x,y);
    }
    void shearAroundOrigin(double my){
        transform(Matrix::shearVOrigin(my));
    }
    void shearAroundPoint(double x,double y,double my){
        translate(-x,-y);
        shearAroundOrigin(my);
        translate(x,y);
    }
    void shearAroundPointWithAxis(double x,double y,double phi,double my){
        translate(-x,-y);
        rotateAroundOrigin(phi);
        shearAroundOrigin(my);
        rotateAroundOrigin(-phi);
        translate(x,y);
    }
    sf::Vector2f getMassCenter() const{
        float sumx=0,sumy=0;
        int vc = va.getVertexCount();
        for(int i=0;i<vc;i++){
            sumx+= va[i].position.x;
            sumy+= va[i].position.y;
        }
        return sf::Vector2f{sumx/vc,sumy/vc};
    }


    
};
int main(){
    PolygonShape original_shape(6);
    PolygonShape shape(original_shape);
    sf::RenderWindow win(sf::VideoMode(800, 600), "CGFX lab3");
    int anim = 4;
    
    sf::Clock clk;
    while(win.isOpen()){
        sf::Event event;
        auto mc = shape.getMassCenter();
        while(win.pollEvent(event)){
            
            if(event.type == sf::Event::Closed){
                win.close();
            }
            if(event.type == sf::Event::KeyPressed){
                switch (event.key.code){
                case sf::Keyboard::Left:
                    shape.translate(-10,0);
                    break;
                case sf::Keyboard::Right:
                    shape.translate(10,0);
                    break;
                case sf::Keyboard::Up:
                    shape.translate(0,-10);
                    break;
                case sf::Keyboard::Down:
                    shape.translate(0,10);
                    break;
                case sf::Keyboard::Q:
                    
                    shape.rotateAroundPoint(mc.x,mc.y,M_PI_4);
                    break;
                case sf::Keyboard::E:
                    
                    shape.rotateAroundPoint(mc.x,mc.y,-M_PI_4);
                    break;
                case sf::Keyboard::W:
                    shape.scaleAroundPoint(mc.x,mc.y,2,2);
                    break;
                case sf::Keyboard::S:
                    shape.scaleAroundPoint(mc.x,mc.y,0.5,0.5);
                    break;
                case sf::Keyboard::D:
                    shape.shearAroundPointWithAxis(mc.x,mc.y,M_PI_4*3,M_SQRT2);
                    break;
                case sf::Keyboard::A:
                    shape.shearAroundPointWithAxis(mc.x,mc.y,3*M_PI_4,-M_SQRT2);
                    break;
                case sf::Keyboard::Space:
                    anim = (anim+1)%5;
                case sf::Keyboard::R:
                    if(event.key.shift)
                        original_shape = PolygonShape(shape);
                    shape = PolygonShape(original_shape);
                    break;    
                default:
                    break;
                }
            }
        }
        
        win.clear();
        sf::Time elapsed = clk.restart();
        float dt=elapsed.asSeconds();
        switch(anim){
        case 0:
            shape.translate(100*dt*0.5,-100*dt);
            break;
        case 1:
            shape.rotateAroundPoint(mc.x,mc.y,M_PI/10.0*dt);
            break;
        case 2:
            shape.scaleAroundPoint(mc.x,mc.y,1+dt/10,1+dt/10);
            break;
        case 3:
            shape.shearAroundPointWithAxis(mc.x,mc.y,3*M_PI_4,M_SQRT2*dt);
            break;
        case 4:
        default:
        break;
        }


        win.draw(shape.va);
        win.display();
    }
    return 0;
}