// complile with g++ gravity_sim.cpp -o gravity_sim -lsfml-graphics -lsfml-window -lsfml-system
// requires sudo apt-get install libsfml-dev
// https://www.sfml-dev.org/tutorials/2.5/start-linux.php for instructions on how to install SFML

// to do
// change solver to RK4
// change how size/colour of objects are set

#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream> // used for scientific notation
using namespace std;


class Body
{
    string name;
    double mass;
    double gMass; // gravitational constant (G) * mass
    double gConstant = 6.6743e-11; 
    float screenOffset;
    float screenScaler;


    sf::CircleShape s;
    sf::Vector2f screenPosition;
    sf::Vector3f position;
    sf::Vector3f velocity;
    sf::Vector3f acceleration;

public:
    Body(int screenSize, int screenSizeAu, string name, float mass, float positionVelocityArray[6] )
    {
        setOffset_setScaler(screenSize,screenSizeAu);
        this->name = name;
        this->position.x = positionVelocityArray[0]*1000; // *1000 to convert from km to m
        this->position.y = positionVelocityArray[1]*1000;
        this->position.z = positionVelocityArray[2]*1000;
        this->velocity.x = positionVelocityArray[3]*1000;
        this->velocity.y = positionVelocityArray[4]*1000;
        this->velocity.z = positionVelocityArray[5]*1000;
        this->acceleration.x = 0; // reset acceleration sum to 0
        this->acceleration.y = 0;
        this->acceleration.z = 0;

        screenPosition.x = positionVelocityArray[0]*screenScaler+screenOffset;
        screenPosition.y = positionVelocityArray[1]*screenScaler+screenOffset;
        // position.z = positionVelocityArray[2]; // to render to window we must use Vector2f so 3d unavailable but we can still do physics in 3d
        this->mass = mass;
        this->gMass = mass*gConstant;  
        
        
        s.setPosition(screenPosition); // change to variable to be set in initialiser
        if (name == "Sun"){
        s.setFillColor(sf::Color::Yellow);
        s.setRadius(8);
        } else if (name == "Earth"){
        s.setFillColor(sf::Color::Green);
        s.setRadius(4);
        } else if (name == "mercury"){
        s.setFillColor(sf::Color::Red);
        s.setRadius(2);
        } else if (name == "venus"){
        s.setFillColor(sf::Color::Magenta);
        s.setRadius(3);
        }else if (name == "mars"){
        s.setFillColor(sf::Color::Red);
        s.setRadius(4);
        } else{
        s.setFillColor(sf::Color::White);
        s.setRadius(5);
        }
    }
    float get_gMass()
    {
        return gMass;
    }

    string get_name()
    {
        return name;
    }

    sf::Vector3f get_position()
    {
        return position;
    }   

    void updateScreenPosition()
    {
        screenPosition.x = position.x*screenScaler+screenOffset;
        screenPosition.y = position.y*screenScaler+screenOffset;
        s.setPosition(screenPosition);
    }

    void render(sf::RenderWindow& wind)
    {
        wind.draw(s);
    }

    void setOffset_setScaler(int screenSize,int screenSizeAu)
    {
        this->screenOffset = screenSize*0.5;
        this->screenScaler = screenSize/screenSizeAu*0.5/1.496e+11; // divided by AU
    }

    void updateSolar(vector<Body> solarSystem,int currentBody)
    {
        acceleration.x = 0; // reset acceleration sum to 0
        acceleration.y = 0;
        acceleration.z = 0;
        for (size_t otherBody = 0; otherBody < solarSystem.size(); otherBody++)
        {
            if (otherBody != currentBody){
                calculateForce(solarSystem[otherBody]);
            }

        }
        applyForce();
        updateScreenPosition();
        

    }

    void calculateForce(Body otherBody)
    {
        sf::Vector3f positionDelta = otherBody.get_position() - position;
        double distance = sqrt(positionDelta.x*positionDelta.x + positionDelta.y*positionDelta.y + positionDelta.z*positionDelta.z);
        
        double inverseDistance = 1.d/distance;
        double inverseDistance3 = inverseDistance * inverseDistance * inverseDistance; // multiplication quicker then division

        acceleration.x += -otherBody.get_gMass() * position.x * inverseDistance3;
        acceleration.y += -otherBody.get_gMass() * position.y * inverseDistance3;
        acceleration.z += -otherBody.get_gMass() * position.z * inverseDistance3;

             
    }

    void applyForce()
    {
        float deltaTime = 60*60*24*0.01; // change to input to easily change 
        // position.x += velocity.x * deltaTime + acceleration.x * deltaTime * deltaTime *.5;
        // position.y += velocity.y * deltaTime + acceleration.y * deltaTime * deltaTime *.5;
        // position.z += velocity.z * deltaTime + acceleration.z * deltaTime * deltaTime *.5;

        // velocity.x += acceleration.x * deltaTime;
        // velocity.y += acceleration.y * deltaTime;
        // velocity.z += acceleration.z * deltaTime;

        position.x += velocity.x * deltaTime + acceleration.x * deltaTime  ;
        position.y += velocity.y * deltaTime + acceleration.y * deltaTime  ;
        position.z += velocity.z * deltaTime + acceleration.z * deltaTime  ;

        velocity.x += acceleration.x * deltaTime;
        velocity.y += acceleration.y * deltaTime;
        velocity.z += acceleration.z * deltaTime;
        
    }


};



int main()
{
    int screenSize = 1000;
    int screenSizeAu = 10;
    // int numberOfBodies = 2;

    // below are values in km . s for Start=2022-05-16 TDB , Stop=2022-06-15, Step=1 (days) 4 with solar system barycenter
    // https://ssd.jpl.nasa.gov/horizons/app.html#/
    float sunArray[]   = {-1.337802732464156E+06,  3.259070687973434E+05,  2.857406953363138E+04, -3.401697178605676E-03, -1.546423174317109E-02,  2.022019805635019E-04};
    Body sun(screenSize,screenSizeAu,"Sun",1.989e30,sunArray);
    float EarthArray[] = {-8.845568359343487E+07, -1.232920159424238E+08,  3.523754350750893E+04,  2.385103124309283E+01, -1.728119023775613E+01,  2.156876333995861E-03};
    Body earth(screenSize,screenSizeAu,"Earth",5.972e24,EarthArray);
    float mercuryArray[] = {-4.967857508918729E+07, -4.590085738340439E+07,  6.851089667090010E+05,  2.374093765344752E+01, -3.301290891958826E+01, -4.874311558794187E+00};
    Body mercury(screenSize,screenSizeAu,"mercury",3.302e23,mercuryArray);
    float venusArray[] = {7.165055875623713E+07, -8.037227570294720E+07, -5.290818498977233E+06,  2.573962145070925E+01,  2.334979765107016E+01, -1.164485971754976E+00};
    Body venus(screenSize,screenSizeAu,"venus",48.685e23,venusArray);
    float marsArray[] = {1.407829778726926E+08, -1.514923030686029E+08, -6.639406796639733E+06,  1.860092566130745E+01,  1.862144154103242E+01, -6.556948326662848E-02};
    Body mars(screenSize,screenSizeAu,"mars",6.4171e23,marsArray);
    float jupiterArray[] = {7.330064286798198E+08, -1.138627564283834E+08, -1.592681184619932E+07,  1.851309562442064E+00,  1.352451544659290E+01, -9.747965584743845E-02};
    Body jupiter(screenSize,screenSizeAu,"jupiter",1.899e27,jupiterArray);
    float saturnArray[] = {1.110969009336197E+09, -9.749872333028338E+08, -2.727994992253995E+07,  5.829459597623179E+00,  7.240774022372236E+00, -3.578312742970602E-01};
    Body saturn(screenSize,screenSizeAu,"saturn",5.685e26,saturnArray);

    // currently moons do not work due to current ODE solver used
    float ioArray[] = {7.326627459121795E+08, -1.136211038571918E+08, -1.592347246995617E+07, -8.176372813170511E+00, -6.848843874649442E-01, -7.585241980527306E-01};
    Body io(screenSize,screenSizeAu,"io",8.93e22,ioArray);
    float moonArray[] = {-8.867706657366812E+07, -1.235791777496837E+08,  3.516301230825484E+04,  2.472717774736751E+01, -1.791624729142814E+01, -9.792200298491238E-02};
    Body moon(screenSize,screenSizeAu,"moon",7.349e22,moonArray);
    

    vector<Body> solarSystem = {sun,earth,mercury,venus,mars,jupiter,saturn};


    sf::RenderWindow window(sf::VideoMode(screenSize, screenSize), "Gravity Simulation");
    window.setFramerateLimit(10000); // dictates max rate of calculations



while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) window.close();
 
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) window.close();
        }
 
 
        window.clear();

        
        
        for (size_t currentBody = 0; currentBody < solarSystem.size(); currentBody++)
        {
            solarSystem[currentBody].updateSolar(solarSystem,currentBody);
            solarSystem[currentBody].render(window);
        }
        
        window.display();

    }


    return 0;
}


