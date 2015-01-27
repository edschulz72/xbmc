#pragma once

#include <string>

class CVDMRegionFeature
{
public:
    enum Type
    {
        tUnknown,
        tDebug,
        tChina,
        tInternation,
    };
    void SetType(const Type type);
    Type GetType( void ) const { return m_type;}

    const std::string& GetWebSite( void ) const { return m_strWebSize; }
    const std::string& GetForum( void ) const { return m_strForum; }
public:
    static CVDMRegionFeature& Get( void );
private:
    CVDMRegionFeature();
    Type m_type;
    std::string m_strWebSize;
    std::string m_strForum;
};
