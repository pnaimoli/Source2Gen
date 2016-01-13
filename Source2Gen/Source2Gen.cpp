#include <fstream>

#include "Source2Gen.hpp"

#include "SchemaClassGenerator.hpp"
#include "SchemaEnumGenerator.hpp"

Source2Gen::Source2Gen(const std::string& genFolder) 
	: m_genFolder(genFolder)
{

}

void Source2Gen::GenerateHeaders()
{
    CreateSchemaBase();

    schema::SchemaSystem* schemaSystem = schema::SchemaSystem::Get();

    std::vector<schema::CSchemaSystemTypeScope*> scopes;

    scopes.push_back(schemaSystem->GlobalTypeScope());
    for (const char * lib : {"client", "server", "worldrenderer",
                             "vgui2", "vguirendersurfaces"})
    {
#ifdef _WIN32
        const std::string filename = std::string(lib) + ".dll";
#else
        const std::string filename = std::string("lib") + lib + ".dylib";
#endif
        scopes.push_back(schemaSystem->FindTypeScopeForModule(filename.c_str()));
    }

    // call the constructor for each one, so our classes will be known.
    std::vector<std::shared_ptr<SchemaClassGenerator>> classGens;
    std::vector<std::shared_ptr<SchemaEnumGenerator>> enumGens;
    for (auto scope : scopes)
    {
        classGens.push_back(std::make_shared<SchemaClassGenerator>(SchemaClassGenerator(scope)));
        enumGens.push_back(std::make_shared<SchemaEnumGenerator>(SchemaEnumGenerator(scope)));
    }

    // Now generate
    for (auto gen : classGens)
        gen->Generate(m_genFolder);
    for (auto gen : enumGens)
        gen->Generate(m_genFolder);
}

void Source2Gen::CreateSchemaBase()
{
	// a base class to inherit from for generated classes that use virtuals
	// it is done like this because Visual Studio sometimes aligns the vtable pointer to 8 bytes
	// and this is the only way to fix it (without #pragma pack)
	std::ofstream out(m_genFolder + "/" + "SchemaBase.hpp", std::ofstream::out);

	if (out.is_open())
	{
		out << "#pragma once" << std::endl;
		out << "namespace schema { class CSchemaClassBinding; }" << std::endl;
		out << "class SchemaBase" << std::endl;
		out << "{" << std::endl;
		out << "public:" << std::endl;
		out << "	virtual schema::CSchemaClassBinding* Schema_DynamicBinding() { return nullptr; }" << std::endl;
		out << "};" << std::endl;

		out.close();
	}
}
