//
//  BoxTest.cpp
//  Box2dTest
//
//  Created by WF on 16/3/16.
//
//

#include "BoxTest.h"

BoxTest::~BoxTest()
{
    CC_SAFE_DELETE(world);//安全释放内存
}
Scene * BoxTest::scene()
{
    auto scene = Scene::create();
    auto layer = BoxTest::create();
    scene->addChild(layer);
    return scene;
}
bool BoxTest::init()
{
    if (!Layer::init()) {
        return false;
    }
    Size visibleSize=Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    
    //初始化物理引擎
    this->initWorld();
    
    setTouchEnabled(true);
    setTouchMode(Touch::DispatchMode::ONE_BY_ONE);
    
    scheduleUpdate();
    return true;
}
bool BoxTest::onTouchBegan(Touch * touch, Event * event)
{
    addNewBoxAtPosition(touch->getLocation());
    //addNewCircleAtPosition(touch->getLocation());
    //addNewJointAtPosition(touch->getLocation());
    return true;
}
void BoxTest::initWorld()
{
    Size s = Director::getInstance()->getVisibleSize();
    //重力参数，向量
    b2Vec2 gravity;
    //设置水平方向和垂直方向上的加速度
    gravity.Set(0.0f, -10.0f);
    //创建世界
    world = new b2World(gravity);
    //是否允许物体休眠，如果允许休眠，可以提高物理世界的处理效率，只有在发生碰撞是才唤醒对象，节省CPU
    world->SetAllowSleeping(true);
    //是否开启连续碰撞，计算机智能把一段连续的时间分成许多离散的时间点，在对每个时间点之间的行为进行机选，如果时间点分割的不够细致，速度较快的两个物体碰撞时就可能产生穿透，连续碰撞将启用特殊的算法避免该现象。
    world->SetContinuousPhysics(true);
    
    //创建物体定义
    b2BodyDef groundBodyDef;
    groundBodyDef.position.Set(0, 0);//设置定义的位置
    //通过物体定义创建地面物体
    b2Body * groundBody = world->CreateBody(&groundBodyDef);
    
    //定义一个有边的形状(边界)
    b2EdgeShape groundBox;
    //底部
    groundBox.Set(b2Vec2(0, 0), b2Vec2(s.width/PTM_PATIO, 0));
    //使用夹具固定形状到物体上
    groundBody->CreateFixture(&groundBox,0);//密度为0
    //顶部
    groundBox.Set(b2Vec2(0, s.height/PTM_PATIO), b2Vec2(s.width/PTM_PATIO, s.height/PTM_PATIO));
    groundBody->CreateFixture(&groundBox,0);
    //左边
    groundBox.Set(b2Vec2(0, s.height/PTM_PATIO), b2Vec2(0, 0));
    groundBody->CreateFixture(&groundBox,0);
    //右边
    groundBox.Set(b2Vec2(s.width/PTM_PATIO,0), b2Vec2(s.width/PTM_PATIO, s.height/PTM_PATIO));
    groundBody->CreateFixture(&groundBox, 0);
    
    
}

void BoxTest::addNewBoxAtPosition(Vec2 p)
{
    log("ADD SPR %0.2f x %0.2f",p.x,p.y);
    
    auto sprite = Sprite::create("BoxA.png");
    sprite->setPosition(Vec2(p.x, p.y));
    addChild(sprite);
    
    //物体定义
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;//物体类型为动态刚体
    bodyDef.position.Set(p.x/PTM_PATIO, p.y/PTM_PATIO);//设置位置，单位 米
    b2Body * body = world->CreateBody(&bodyDef);//创建物体
    body->SetUserData(sprite);//将精灵放置到物体的UserData属性中，这样便于我们从物体中获取相关联的物体。
    
    //定义两米见方的盒子形状
    b2PolygonShape dynamicBox;
    dynamicBox.SetAsBox(1, 1);//盒子左下角为(0,0),中心点为(1,1),这样就是2米见方
    
    //夹具定义
    b2FixtureDef fixtureDef;
    //设置夹具形状
    fixtureDef.shape = &dynamicBox;
    //设置密度
    fixtureDef.density = 1.0f;
    //设置摩擦系数 范围：0.0~1.0
    fixtureDef.friction = 0.3f;
    fixtureDef.restitution = 0.5f;
    //使用夹具固定形状到物体上,这样物体就有了形状
    body->CreateFixture(&fixtureDef);
    
}
void BoxTest::addNewCircleAtPosition(Vec2 p)
{
    auto sp = Sprite::create("circle.png");
    sp->setPosition(p);
    addChild(sp);
    
    b2BodyDef bodyDef;//物体定义
    bodyDef.type = b2_dynamicBody;//设置类型为动态刚体
    bodyDef.position.Set(p.x/PTM_PATIO, p.y/PTM_PATIO);//设置物体位置
    b2Body * body = world->CreateBody(&bodyDef);//创建物体
    body->SetUserData(sp);
    
    b2CircleShape circle;//创建一个圆形
    circle.m_radius = 20.0/PTM_PATIO;//设置半径
    
    //定义fixture  夹具
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &circle;//形状
    fixtureDef.density = 1;//密度
    fixtureDef.restitution = 0.7;//弹性体的复原——弹性
    fixtureDef.friction = 0.4;//摩擦力
    
    body->CreateFixture(&fixtureDef);
}
void BoxTest::addNewJointAtPosition(Vec2 p)
{
    //创建精灵A
    auto spA = Sprite::create("BoxA.png");
    spA->setPosition(p);
    addChild(spA);
    //动态物体A定义
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(p.x/PTM_PATIO, p.y/PTM_PATIO);
    b2Body * bodyA = world->CreateBody(&bodyDef);
    bodyA->SetUserData(spA);
    
    //创建精灵B
    auto spB = Sprite::create("BoxB.png");
    spB->setPosition(p+Vec2(100, -100));
    addChild(spB);
    
    //动态物体B定义
    bodyDef.type = b2_dynamicBody;
    Vec2 posB = spB->getPosition();
    bodyDef.position.Set(posB.x/PTM_PATIO, posB.y/PTM_PATIO);
    
    b2Body * bodyB = world->CreateBody(&bodyDef);
    bodyB->SetUserData(spB);
    
    //定义2米见方的盒子形状
    b2PolygonShape dynamicBox;
    dynamicBox.SetAsBox(1, 1);
    
    //动态物体夹具定义
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &dynamicBox;//形状
    fixtureDef.density = 1.0f;//密度
    fixtureDef.friction = 0.3f;//摩擦系数
    fixtureDef.restitution = 0.4f;//弹性
    
    //使用夹具固定形状到物体上
    bodyA->CreateFixture(&fixtureDef);
    bodyB->CreateFixture(&fixtureDef);
    
    //距离关节定义
    b2DistanceJointDef joinDef;
    
    //初始化定义 物体1 物体2 物体1的锚点 物体2的锚点（世界坐标）
    joinDef.Initialize(bodyA, bodyB, bodyA->GetWorldCenter(), bodyB->GetWorldCenter());
    
    joinDef.collideConnected = true;//允许相连的物体碰撞
    bodyA->GetWorld()->CreateJoint(&joinDef);
    
    /*
     Box2D v2版本中定义了很多关节。这些关节包括：
     距离关节:两个物体之间保持固定的距离。每个物体上的点称为锚点。关节定义是b2DistanceJointDef。
     旋转关节:允许物体围绕公共点旋转。关节定义是b2RevoluteJointDef。
     平移关节:两个物体之间的相对旋转是固定的，它们可以沿着一个坐标轴进行平移。关节定义是b2PrismaticJointDef。
     焊接关节:可以把物体固定在相同方向上。关节定义是b2WeldJointDef。
     滑轮关节:滑轮关节用于创建理想的滑轮，两个物体位于绳子两端，绳子通过某个固定点（滑轮的位置）将两个物体连接起来。这样当一个物体升起时，另一个物体就会下降。滑轮两端的绳子总长度不变。关节定义是b2PulleyJointDef。
     摩擦关节:降低两个物体之间的相对运动。关节定义是b2FrictionJointDef。
     齿轮关节:控制其它两个关节（旋转关节或者平移关节），其中一个的运动会影响另一个。关节定义是b2GearJointDef。
     鼠标关节:点击物体上任意一个点可以在世界范围内进行拖动。关节定义是b2MouseJointDef。
     */
}
void BoxTest::update(float dt)//同步物理引擎中的物体与精灵的位置
{   //模拟世界
    /*
     Box2D 中有一些数学代码构成的积分器(integrator)，积分器在离散的时间点上模
     拟物理方程，它将与游戏动画循环一同运行。所以我们需要为 Box2D 选取一个时间
     步，通常来说游戏物理引擎需要至少 60Hz 的速度，也就是 1/60 的时间步。你可
     以使用更大的时间步，但是你必须更加小心地为你的世界调整定义。我们也不喜欢时
     间步变化得太大，所以不要把时间步关联到帧频(除非你真的必须这样做)。直截了当
     地，这个就是时间步：
     float32 timeStep = 1.0f / 60.0f;
     
         除了积分器之外，Box2D 中还有约束求解器(constraint solver)。约束求
     解器用于解决模拟中的所有约束，一次一个。单个的约束会被完美的求解，然而当我
     们求解一个约束的时候，我们就会稍微耽误另一个。要得到良好的解，我们需要迭代
     所有约束多次。建议的 Box2D 迭代次数是 10 次。
         可以按自己的喜好去调整这个数，但要记得它是速度与质量之间的平衡。更少的
     迭代会增加性能并降低精度，同样地，更多的迭代会减少性能但提高模拟质量。
     */
    float timeStep = 1/60.0f;//时间步

    /*********  一个时间步遍历8次约束  *********/
    int32 velocityIterations = 10;//遍历10次约束
    int32 positionIterations = 1;//一个时间步
    
    
    world->Step(timeStep, velocityIterations, positionIterations);
    
    for (b2Body * b = world->GetBodyList(); b; b=b->GetNext())
    {
        if (b->GetUserData() != nullptr)
        {
            Sprite * sprite = (Sprite*)b->GetUserData();
            sprite->setPosition(Vec2(b->GetPosition().x*PTM_PATIO, b->GetPosition().y*PTM_PATIO));
            sprite->setRotation(-1*CC_RADIANS_TO_DEGREES(b->GetAngle()));//CC_RADIANS_TO_DEGREES 弧度转换为角度
        }
    }
}











