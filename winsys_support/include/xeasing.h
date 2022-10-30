//********************************************************************************//
//    Chức năng tính toán các hiệu ứng easing (animation)                         //
//    Author: Thuong.NV         Date : 6/5/2022                                   //
//********************************************************************************//

//==================================================================================
// Chỉ nên sử dụng nó với một số chức năng cơ bản, đối với các hiện tượng vật lý nên
// sử dụng các thành phần vật lý để tạo ra cảm giác thật                            
//==================================================================================


#ifndef GLEASING_H
#define GLEASING_H

#include <iostream>
#include <stdarg.h>
#include <vector>
#include <combaseapi.h>
#include <memory>

#define EASING_PI               3.14159265359f
#define EASING_EPSILON          0.001f

#define EASING_STANDTIME_START  0.f
#define EASING_STANDTIME_END    1.f

#define S2MS(s)         (s  * 1000.f)
#define MS2S(ms)        (ms / 1000.f)

/*
Easing functions : specify the rate of change of a parameter over time.             

Bao gồm tính toán các hiệu ứng easing:
        +   Back                                                                    
        +   Quint                                                                   
        +   Elastic                                                                 
        +   Quart                                                                   

Các phương thức tính toán này dựa vào toán học và thường được áp dụng vào vị trí    

Option:
    EaseIn   : Interpolation follows the mathematical formula associated with the   
               easing function.                                                     
    EaseOut  : Interpolation follows 100% interpolation minus the output of the     
               formula associated with the easing function.                         
    EaseInOut: Interpolation uses EaseIn for the first half of the animation and    
               EaseOut for the second half.                                         
*/


enum class EaseMode
{
    In,
    Out,
    InOut
};

enum class EaseType
{
    Back   ,
    Quint  ,
    Elastic,
    Quart  ,
    Bounce ,
    Expo   ,
};
//==================================================================================
//⮟⮟ Hàm bổ trợ                                                                    
//==================================================================================

static float EasingClamp(float x, float min, float max)
{
    return (x <= min) ? min : (x >= max ? max : x);
}

static float EasingHardMap(float x, float min_s, float max_s, float min_d, float max_d)
{
    if (fabs(max_s - min_s) <= EASING_EPSILON) return min_d;

    float value = ((x - min_s)*(max_d - min_d)) / (max_s - min_s) + min_d;
    return EasingClamp(value, min_d, max_d);
}

static float EasingSoftMap(float x, float min_s, float max_s, float min_d, float max_d)
{
    float A = (max_s - x);
    float B = (min_s - x);

    if      (fabs(A) <= EASING_EPSILON) return max_d;
    else if (fabs(B) <= EASING_EPSILON) return min_d;

    if (fabs(A - B) <= EASING_EPSILON)
    {
        return min_d;
    }

    float value = (A*min_d - B*max_d) / (A - B);
    return value;
}

//==================================================================================
//⮟⮟ Triển khai hàm  EASING BACK //t : 0->1                                        
//==================================================================================
static float EaseInBack(float t)
{
    const float c1 = 1.70158f;
    const float c3 = c1 + 1;

    return c3*t*t*t - c1*t*t;
}

static float EaseOutBack(float t)
{
    const float c1 = 1.70158f;
    const float c3 = c1 + 1;

    return 1 + c3 * std::powf(t - 1, 3) + c1 * std::powf(t - 1, 2);
}

static float EaseInOutBack(float t)
{
    const float c1 = 1.70158f;
    const float c2 = c1 * 1.525f;

    return t < 0.5
        ? (std::powf(2 * t, 2) * ((c2 + 1) * 2 * t - c2)) / 2
        : (std::powf(2 * t - 2, 2) * ((c2 + 1) * (t * 2 - 2) + c2) + 2) / 2;
}

//==================================================================================
//⮟⮟ Triển khai hàm  EASING Quint //t : 0->1                                       
//==================================================================================
static float EaseInQuint(float t)
{
    return t * t * t * t * t;
}

static float EaseOutQuint(float t)
{
    return 1 - std::powf(1 - t, 5);
}

static float EaseInOutQuint(float t)
{
    return t < 0.5 ? 16 * t * t * t * t * t : 1 - std::powf(-2 * t + 2, 5) / 2;
}


//==================================================================================
//⮟⮟ Triển khai hàm  EASING Elastic //t : 0->1                                       
//==================================================================================
static float EaseInElastic(float t)
{
    const float c4 = (2 * EASING_PI) / 3.f;

    return t == 0
        ? 0
        : t == 1
        ? 1
        : -std::powf(2, 10 * t - 10) * sin((t * 10 - 10.75f) * c4);
}

static float EaseOutElastic(float t)
{
    const float c4 = (2 * EASING_PI) / 3.f;

    return t == 0
        ? 0
        : t == 1
        ? 1
        : std::powf(2, -10 * t) * sin((t * 10 - 0.75f) * c4) + 1;
}

static float EaseInOutElastic(float t)
{
    const float c5 = (2 * EASING_PI) / 4.5f;

    return t == 0
        ? 0
        : t == 1
        ? 1
        : t < 0.5
        ? -(std::powf(2,  20 * t - 10) * sin((20 * t - 11.125f) * c5)) / 2
        :  (std::powf(2, -20 * t + 10) * sin((20 * t - 11.125f) * c5)) / 2 + 1;
}

//==================================================================================
//⮟⮟ Triển khai hàm  EASING Quart //t : 0->1                                       
//==================================================================================
static float EaseInQuart(float t)
{
    return t * t * t * t;
}

static float EaseOutQuart(float t)
{
    return 1 - std::powf(1 - t, 4);
}

static float EaseInOutQuart(float t)
{
    return t < 0.5 ? 8 * t * t * t * t : 1 - std::powf(-2 * t + 2, 4) / 2;
}

//==================================================================================
//⮟⮟ Triển khai hàm  EASING Bounce //t : 0->1                                       
//==================================================================================

static float EaseOutBounce(float t)
{
    const float  n1 = 7.5625f;
    const float  d1 = 2.75f;

    if (t < 1 / d1) {
        return n1 * t * t;
    } else if (t < 2.f / d1) {
        return n1 * (t -= 1.5f / d1) * t + 0.75f;
    } else if (t < 2.5f / d1) {
        return n1 * (t -= 2.25f / d1) * t + 0.9375f;
    } else {
        return n1 * (t -= 2.625f / d1) * t + 0.984375f;
    }
}

static float EaseInBounce(float t)
{
    return 1 - EaseOutBounce(1 - t);
}

static float EaseInOutBounce(float t)
{
    return t < 0.5f
        ? (1 - EaseOutBounce(1 - 2 * t)) / 2
        : (1 + EaseOutBounce(2 * t - 1)) / 2;
}

//==================================================================================
//⮟⮟ Triển khai hàm  EASING Expo //t : 0->1                                       
//==================================================================================

static float EaseOutExpo(float t)
{
    return t == 0 ? 0 : std::powf(2, 10 * t - 10);
}

static float EaseInExpo(float t)
{
    return t == 1 ? 1 : 1 - std::powf(2, -10 * t);
}

static float EaseInOutExpo(float t)
{
    return t == 0
        ? 0
        : t == 1
        ? 1
        : t < 0.5 ? std::powf(2, 20 * t - 10) / 2
        : (2 - std::powf(2, -20 * t + 10)) / 2;
}


// Return : Giá trị trong khoảng [vfrom -> vto] tại thời điểm t
static float CallEasingBack(EaseMode  mode      ,// Chế độ
                            float     t         ,// Thời điểm t từ lúc tính
                            float     vfrom     ,// Giá trị bắt đầu
                            float     vto       ,// Giá trị kết thúc
                            float     duration)  // Khoảng thời gian diễn ra
{
    if (t >= duration) return vto;

    float t1  = EasingHardMap(t, 0.f, duration, EASING_STANDTIME_START, EASING_STANDTIME_END);

    float vt1 = 0.f;
    if (mode == EaseMode::Out)
    {
        vt1 = EaseOutBack(t1);
    }
    else if (mode == EaseMode::InOut)
    {
        vt1 = EaseInOutBack(t1);
    }
    else
    {
        vt1 = EaseInBack(t1);
    }

    float value = EasingSoftMap(vt1, EASING_STANDTIME_START, EASING_STANDTIME_END, vfrom, vto);
    return value;
}

// Return : Giá trị trong khoảng [vfrom -> vto] tại thời điểm t
static float CallEasingQuint(EaseMode  mode      ,// Chế độ
                               float     t         ,// Thời điểm t từ lúc tính
                               float     vfrom     ,// Giá trị bắt đầu
                               float     vto       ,// Giá trị kết thúc
                               float     duration)  // Khoảng thời gian diễn ra
{
    if (t >= duration) return vto;

    float t1  = EasingHardMap(t, 0.f, duration, EASING_STANDTIME_START, EASING_STANDTIME_END);

    float vt1 = 0.f;
    if (mode == EaseMode::Out)
    {
        vt1 = EaseOutQuint(t1);
    }
    else if (mode == EaseMode::InOut)
    {
        vt1 = EaseInOutQuint(t1);
    }
    else
    {
        vt1 = EaseInQuint(t1);
    }

    float value = EasingSoftMap(vt1, EASING_STANDTIME_START, EASING_STANDTIME_END, vfrom, vto);
    return value;
}

// Return : Giá trị trong khoảng [vfrom -> vto] tại thời điểm t
static float CallEasingElastic(EaseMode  mode      ,// Chế độ
                               float     t         ,// Thời điểm t từ lúc tính
                               float     vfrom     ,// Giá trị bắt đầu
                               float     vto       ,// Giá trị kết thúc
                               float     duration)  // Khoảng thời gian diễn ra
{
    if (t >= duration) return vto;

    float t1  = EasingHardMap(t, 0.f, duration, EASING_STANDTIME_START, EASING_STANDTIME_END);

    float vt1 = 0.f;
    if (mode == EaseMode::Out)
    {
        vt1 = EaseOutElastic(t1);
    }
    else if (mode == EaseMode::InOut)
    {
        vt1 = EaseInOutElastic(t1);
    }
    else
    {
        vt1 = EaseInElastic(t1);
    }

    float value = EasingSoftMap(vt1, EASING_STANDTIME_START, EASING_STANDTIME_END, vfrom, vto);
    return value;
}


// Return : Giá trị trong khoảng [vfrom -> vto] tại thời điểm t
static float CallEasingBounce(EaseMode  mode      ,// Chế độ
                              float     t         ,// Thời điểm t từ lúc tính
                              float     vfrom     ,// Giá trị bắt đầu
                              float     vto       ,// Giá trị kết thúc
                              float     duration)  // Khoảng thời gian diễn ra
{
    if (t >= duration) return vto;

    float t1  = EasingHardMap(t, 0.f, duration, EASING_STANDTIME_START, EASING_STANDTIME_END);

    float vt1 = 0.f;
    if (mode == EaseMode::Out)
    {
        vt1 = EaseOutBounce(t1);
    }
    else if (mode == EaseMode::InOut)
    {
        vt1 = EaseInOutBounce(t1);
    }
    else
    {
        vt1 = EaseInBounce(t1);
    }

    float value = EasingSoftMap(vt1, EASING_STANDTIME_START, EASING_STANDTIME_END, vfrom, vto);
    return value;
}

//===================================================================================
// Class EasingBase : Lớp cơ sở các hiệu ứng Easing liên quan                        
//===================================================================================

interface EasingBase
{
	typedef std::shared_ptr<EasingBase> easingbase_ptr;
};

interface EasingAction : public EasingBase
{
	typedef std::shared_ptr<EasingAction> easingaction_ptr;

protected:
    virtual float  EaseIn(const float t, const float duration, const float from, const float to)   = 0;
    virtual float  EaseOut(const float t, const float duration, const float from, const float to)  = 0;
    virtual float  EaseInOut(const float t, const float duration, const float from, const float to)= 0;

    friend class EasingEngine;
};

class EasingObject : public EasingBase
{
//↓ disable create object EasingObject inheritance - use pointer from CreateIntanse
private:
    //struct secret {};
    //virtual void impossible(secret) = 0; // cannot override this in subclasses
    //                                     // because the parameter type is private

    //template <class X> class AImpl : public X
    //{
    //    void impossible(secret) {}; // a nested class can access private members
    //};
//↑ disable create object EasingObject inheritance - use pointer from CreateIntanse

public:
    virtual EaseType		GetType() = 0;
	//virtual easingbase_ptr  CreateIntanseAction() = 0;

	template<typename T>
	static easingbase_ptr	CreateInstanseActionex()
	{
		return std::make_shared<T>();
	}
};

class EasingBack : public EasingAction, EasingObject
{
public:
    virtual EaseType GetType() { return EaseType::Back; }
	//virtual easingbase_ptr CreateIntanseAction()
	//{
	//	return std::make_shared<EasingBase>(new EasingBack());
	//}

private:
    virtual float EaseIn(const float t, const float duration, const float from, const float to)
    {
        float t1    = EasingHardMap(t, 0.f, duration, EASING_STANDTIME_START, EASING_STANDTIME_END);
        float vt1   = EaseInBack(t1);
        float value = EasingSoftMap(vt1, EASING_STANDTIME_START, EASING_STANDTIME_END, from, to);

        return value;
    }

    virtual float EaseOut(const float t, const float duration, const float from, const float to)
    {
        float t1    = EasingHardMap(t, 0.f, duration, EASING_STANDTIME_START, EASING_STANDTIME_END);
        float vt1   = EaseOutBack(t1);
        float value = EasingSoftMap(vt1, EASING_STANDTIME_START, EASING_STANDTIME_END, from, to);

        return value;
    }

    virtual float EaseInOut(const float t, const float duration, const float from, const float to)
    {
        float t1    = EasingHardMap(t, 0.f, duration, EASING_STANDTIME_START, EASING_STANDTIME_END);
        float vt1   = EaseInOutBack(t1);
        float value = EasingSoftMap(vt1, EASING_STANDTIME_START, EASING_STANDTIME_END, from, to);

        return value;
    }
};

class EasingQuint : public EasingAction, EasingObject
{
public:
    EasingQuint()
    {

    }
public:
    virtual EaseType GetType() { return EaseType::Quint; }

private:
    virtual float EaseIn(const float t, const float duration, const float from, const float to)
    {
        float t1    = EasingHardMap(t, 0.f, duration, EASING_STANDTIME_START, EASING_STANDTIME_END);
        float vt1   = EaseInQuint(t1);
        float value = EasingSoftMap(vt1, EASING_STANDTIME_START, EASING_STANDTIME_END, from, to);

        return value;
    }

    virtual float EaseOut(const float t, const float duration, const float from, const float to)
    {
        float t1    = EasingHardMap(t, 0.f, duration, EASING_STANDTIME_START, EASING_STANDTIME_END);
        float vt1   = EaseOutQuint(t1);
        float value = EasingSoftMap(vt1, EASING_STANDTIME_START, EASING_STANDTIME_END, from, to);

        return value;
    }

    virtual float EaseInOut(const float t, const float duration, const float from, const float to)
    {
        float t1    = EasingHardMap(t, 0.f, duration, EASING_STANDTIME_START, EASING_STANDTIME_END);
        float vt1   = EaseInOutQuint(t1);
        float value = EasingSoftMap(vt1, EASING_STANDTIME_START, EASING_STANDTIME_END, from, to);

        return value;
    }
};

class EasingElastic :public EasingAction, EasingObject
{
public:
    virtual EaseType GetType() { return EaseType::Elastic; }

private:
    virtual float EaseIn(const float t, const float duration, const float from, const float to)
    {
        float t1    = EasingHardMap(t, 0.f, duration, EASING_STANDTIME_START, EASING_STANDTIME_END);
        float vt1   = EaseInElastic(t1);
        float value = EasingSoftMap(vt1, EASING_STANDTIME_START, EASING_STANDTIME_END, from, to);

        return value;
    }

    virtual float EaseOut(const float t, const float duration, const float from, const float to)
    {
        float t1    = EasingHardMap(t, 0.f, duration, EASING_STANDTIME_START, EASING_STANDTIME_END);
        float vt1   = EaseOutElastic(t1);
        float value = EasingSoftMap(vt1, EASING_STANDTIME_START, EASING_STANDTIME_END, from, to);

        return value;
    }

    virtual float EaseInOut(const float t, const float duration, const float from, const float to)
    {
        float t1    = EasingHardMap(t, 0.f, duration, EASING_STANDTIME_START, EASING_STANDTIME_END);
        float vt1   = EaseInOutElastic(t1);
        float value = EasingSoftMap(vt1, EASING_STANDTIME_START, EASING_STANDTIME_END, from, to);

        return value;
    }
};

class EasingQuart : public EasingAction, EasingObject
{
public:
    virtual EaseType GetType() { return EaseType::Quart; }

private:
    virtual float EaseIn(const float t, const float duration, const float from, const float to)
    {
        float t1    = EasingHardMap(t, 0.f, duration, EASING_STANDTIME_START, EASING_STANDTIME_END);
        float vt1   = EaseInQuart(t1);
        float value = EasingSoftMap(vt1, EASING_STANDTIME_START, EASING_STANDTIME_END, from, to);

        return value;
    }

    virtual float EaseOut(const float t, const float duration, const float from, const float to)
    {
        float t1    = EasingHardMap(t, 0.f, duration, EASING_STANDTIME_START, EASING_STANDTIME_END);
        float vt1   = EaseOutQuart(t1);
        float value = EasingSoftMap(vt1, EASING_STANDTIME_START, EASING_STANDTIME_END, from, to);

        return value;
    }

    virtual float EaseInOut(const float t, const float duration, const float from, const float to)
    {
        float t1    = EasingHardMap(t, 0.f, duration, EASING_STANDTIME_START, EASING_STANDTIME_END);
        float vt1   = EaseInOutQuart(t1);
        float value = EasingSoftMap(vt1, EASING_STANDTIME_START, EASING_STANDTIME_END, from, to);

        return value;
    }
};

class EasingBounce : public EasingAction, EasingObject
{
public:
    virtual EaseType GetType() { return EaseType::Bounce; }

private:
    virtual float EaseIn(const float t, const float duration, const float from, const float to)
    {
        float t1    = EasingHardMap(t, 0.f, duration, EASING_STANDTIME_START, EASING_STANDTIME_END);
        float vt1   = EaseInBounce(t1);
        float value = EasingSoftMap(vt1, EASING_STANDTIME_START, EASING_STANDTIME_END, from, to);

        return value;
    }

    virtual float EaseOut(const float t, const float duration, const float from, const float to)
    {
        float t1    = EasingHardMap(t, 0.f, duration, EASING_STANDTIME_START, EASING_STANDTIME_END);
        float vt1   = EaseOutBounce(t1);
        float value = EasingSoftMap(vt1, EASING_STANDTIME_START, EASING_STANDTIME_END, from, to);

        return value;
    }

    virtual float EaseInOut(const float t, const float duration, const float from, const float to)
    {
        float t1    = EasingHardMap(t, 0.f, duration, EASING_STANDTIME_START, EASING_STANDTIME_END);
        float vt1   = EaseInOutBounce(t1);
        float value = EasingSoftMap(vt1, EASING_STANDTIME_START, EASING_STANDTIME_END, from, to);

        return value;
    }
};

class EasingExpo : public EasingAction, EasingObject
{
public:
    virtual EaseType GetType() { return EaseType::Expo; }

private:
    virtual float EaseIn(const float t, const float duration, const float from, const float to)
    {
        float t1    = EasingHardMap(t, 0.f, duration, EASING_STANDTIME_START, EASING_STANDTIME_END);
        float vt1   = EaseInExpo(t1);
        float value = EasingSoftMap(vt1, EASING_STANDTIME_START, EASING_STANDTIME_END, from, to);

        return value;
    }

    virtual float EaseOut(const float t, const float duration, const float from, const float to)
    {
        float t1    = EasingHardMap(t, 0.f, duration, EASING_STANDTIME_START, EASING_STANDTIME_END);
        float vt1   = EaseOutExpo(t1);
        float value = EasingSoftMap(vt1, EASING_STANDTIME_START, EASING_STANDTIME_END, from, to);

        return value;
    }

    virtual float EaseInOut(const float t, const float duration, const float from, const float to)
    {
        float t1    = EasingHardMap(t, 0.f, duration, EASING_STANDTIME_START, EASING_STANDTIME_END);
        float vt1   = EaseInOutExpo(t1);
        float value = EasingSoftMap(vt1, EASING_STANDTIME_START, EASING_STANDTIME_END, from, to);

        return value;
    }
};

//===================================================================================
// Class EasingEngine : Implement execute easing                                     
//===================================================================================

class EasingDataBase : EasingBase
{
public:
	easingbase_ptr   action;
	EaseType         type;
	EaseMode         mode;
	float            from;
	float            to;

public:
	EasingDataBase(easingbase_ptr action, EaseType type, 
		EaseMode mode, float from , float to)
	{
		this->action = action;
		this->type	 = type	 ;
		this->mode	 = mode	 ;
		this->from	 = from	 ;
		this->to	 = to	 ;
	}
};

class EasingEngine : EasingBase
{
private:
    // common setup property
    float                    cumulativeTime;
    bool                     pause;
    float                    duration;

    // data list contain
    std::vector<EasingDataBase> m_data_list;
    std::vector<float>          m_data_value;

public:

    virtual void Reset() { cumulativeTime = 0.f; }
    virtual void Start() { this->Reset(); pause = false; }
    virtual void Pause() { pause = true; }
    virtual void Continue() { pause = false; }
    virtual bool IsActive() { return !pause; }
    virtual void Setup(float _durationsecond)
    {
        pause    = true;
        duration = S2MS(_durationsecond);

        this->Reset();
    }

    virtual bool AddExec(EaseType type, EaseMode mode, float _from, float _to)
    {
        EasingAction::easingaction_ptr action = NULL;

        switch (type)
        {
        case EaseType::Back:
            action = std::make_shared<EasingBack>();
            break;
        case EaseType::Quint:
            action = std::make_shared<EasingQuint>();
            break;
        case EaseType::Elastic:
            action = std::make_shared<EasingElastic>();
            break;
        case EaseType::Quart:
            action = std::make_shared<EasingQuart>();
            break;
        case EaseType::Bounce:
            action = std::make_shared<EasingBounce>();
            break;
        case EaseType::Expo:
            action = std::make_shared<EasingExpo>();
            break;
        default:
            break;
        }

        if (!action)
        {
            std::cout << "[err] : type not support !" << std::endl;
            return false;
        }

        m_data_list.emplace_back(EasingDataBase{ action, type, mode, _from, _to });
        m_data_value.emplace_back(_from);

        return true;
    }

    //==================================================================================
    // Thực hiện tính toán giá trị animation easing với đầu vào là thời điểm t          
    // t : Giá trị đầu vào tính theo millisecond
    //==================================================================================
    virtual void Update(float t)
    {
        if (pause)
            return;

        if (cumulativeTime <= duration)
        {
            cumulativeTime += t;
        }
        else
        {
            pause = true;
            return;
        }

        float value = 0; float from = 0; float to = 0;
        EaseMode mode = EaseMode::In;

        for (int i = 0; i < m_data_list.size(); i++)
        {
            from = m_data_list[i].from;
            to   = m_data_list[i].to;
            mode = m_data_list[i].mode;

            value = to; //default value

            auto _action = std::static_pointer_cast<EasingAction>(m_data_list[i].action);

            if (pause || !_action)
            {
                m_data_value[i] = value;
                continue;
            }

            if (mode == EaseMode::Out)
            {
                value = _action->EaseOut(cumulativeTime, duration, from, to);
            }
            else if (mode == EaseMode::InOut)
            {
                value = _action->EaseInOut(cumulativeTime, duration, from, to);
            }
            else
            {
                value = _action->EaseIn(cumulativeTime, duration, from, to);
            }
            m_data_value[i] = value;
        }
    }

    virtual float Exec(const int& i) const
    {
        return m_data_value[i];
    }

    virtual float operator[](const int& i) const
    {
        return this->Exec(i);
    }

};


#endif // !GLEASING_H
