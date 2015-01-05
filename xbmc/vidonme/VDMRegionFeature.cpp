#include "VDMRegionFeature.h"
#include <cassert>

CVDMRegionFeature& CVDMRegionFeature::Get( void )
{
    static CVDMRegionFeature inst;
    return inst;
}

CVDMRegionFeature::CVDMRegionFeature( void )
    : m_type(tUnknown)
{
    SetType(tInternation);
}

void CVDMRegionFeature::SetType(const Type type)
{
    m_type = type;
    switch (type)
    {
#if 0
    case tUnknown:        
    case tDebug:
#endif
    case tChina:
        m_strWebSize        = "http://vidonme.cn";
        m_strForum          = "http://forum.vidonme.cn";
        break;
    case tInternation:
        m_strWebSize        = "http://vidon.me";
        m_strForum          = "http://forum.vidon.me";
        break;
    default:
        assert(false && "Invalide RegionFeature::Type");
    }
}
