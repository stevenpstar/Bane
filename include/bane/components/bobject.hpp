//Bane Object = Bobject
#ifndef BANE_BOBJECT
#define BANE_BOBJECT
class Camera;
class Bobject
{
  public:
    virtual void Render(Camera* camera);
};
#endif
