#include <algorithm>
#include <sstream>

#include "Schema.hpp"
#include "SchemaSystem.hpp"

#include "Address.hpp"

#include "Interface.hpp"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wignored-attributes"

namespace schema
{
	int CSchemaType::GetSize()
	{
		return (getVirtual<unsigned int(__thiscall*)(CSchemaType*)>(this, s_getSizeIndex))(this);
	}

	int CSchemaType::GetAlignment()
	{
		return (getVirtual<unsigned int(__thiscall*)(CSchemaType*)>(this, s_getAlignmentIndex))(this);
	}

	CSchemaType* CSchemaType::GetInnerType()
	{
		return (getVirtual<CSchemaType* (__thiscall*)(CSchemaType*)>(this, s_getInnerTypeIndex))(this);
	}

	CSchemaType* CSchemaType::GetInnermostType()
	{
		return (getVirtual<CSchemaType* (__thiscall*)(CSchemaType*)>(this, s_getInnermostTypeIndex))(this);
	}

	bool CSchemaType::IsA(CSchemaType* schemaType)
	{
		return (getVirtual<bool(__thiscall*)(CSchemaType*, CSchemaType*)>(this, s_isAIndex))(this, schemaType);
	}

	const char* CSchemaType::GetName()
	{
		//return Address(this).get(0 + sizeof(void*)).to<const char*>();
		return m_Name;
	}

	std::string CSchemaType::GetBaseName()
	{
		if (GetTypeCategory() == CSchemaType::Schema_Atomic && GetAtomicCategory() == Atomic_T)
			return GetName();

		bool isArray = GetTypeCategory() == CSchemaType::Schema_FixedArray;
		bool isPointer = GetTypeCategory() == CSchemaType::Schema_Ptr;

		std::string nakedTypeName = GetName();

		if (isArray)
			nakedTypeName = nakedTypeName.substr(0, nakedTypeName.find_last_of("["));

		if (isPointer)
			nakedTypeName = nakedTypeName.substr(0, nakedTypeName.find_last_of("*"));

		CSchemaType* nakedType = m_TypeScope->FindSchemaTypeByName(nakedTypeName.c_str());

		if (nakedType && nakedType != this)
			nakedTypeName = nakedType->GetBaseName();

		return nakedTypeName;
	}

	CSchemaType* CSchemaType::GetUnderlyingType()
	{
		if (!m_TypeScope)
			return nullptr;

		CSchemaType* underlyingType = m_TypeScope->FindSchemaTypeByName(GetBaseName().c_str());

		if (underlyingType != this)
			return underlyingType;

		return nullptr;
	}

	bool CSchemaClassInfo::GetMetaStrings(const char* metaName, std::vector<const char**>& strings)
	{
		auto& metadata = m_Metadata.m_StaticEntries;

		if (!metadata.data || !metadata.m_size)
			return false;

		SchemaMetadataEntryData_t* metaIterator = metadata.data;
		SchemaMetadataEntryData_t* metaEnd = metadata.data + metadata.m_size;

		do
		{
			metaIterator = std::find_if(metaIterator, metadata.data + metadata.m_size,
				[metaName](SchemaMetadataEntryData_t& entry)
			{
				if (!entry.m_Name.data || !entry.m_Name.size)
					return false;

				//DOTA_LOG("%s", entry.m_Name.data);

				return !strcmp(metaName, entry.m_Name.data);
			});

			if (metaIterator != metaEnd && metaIterator->m_Name.data && metaIterator->unaccounted)
			{
				strings.push_back((const char**)metaIterator->unaccounted);
				++metaIterator;
			}

		} while (metaIterator != metaEnd);

		return !strings.empty();
	}

	unsigned int CSchemaClassInfo::CalculateInheritanceDataSize(bool ignoreVirtuals)
	{
		if (!m_BaseClasses.data || !m_BaseClasses.m_size)
			return 0;

		unsigned int totalSize = 0;

		bool foundVirtuals = false;

		for (auto i = m_BaseClasses.data; i != m_BaseClasses.data + m_BaseClasses.m_size; ++i)
		{
			if (!i->m_pClass)
				continue;

			totalSize += i->m_pClass->m_nSizeOf;

			if (i->m_pClass->m_ClassFlags & SCHEMA_CLASS_HAS_VIRTUAL_MEMBERS)
				foundVirtuals = true;
		}

		if (foundVirtuals && ignoreVirtuals)
			totalSize -= sizeof(void*);

		return totalSize;
	}

	bool CSchemaClassInfo::DependsOn(CSchemaClassInfo* other)
	{
		return InheritsFrom(other) || UsesClass(other);
	}

	bool CSchemaClassInfo::InheritsFrom(CSchemaClassInfo* other)
	{
		if (!m_BaseClasses.data || !m_BaseClasses.m_size)
			return false;

		for (auto i = m_BaseClasses.data; i != m_BaseClasses.data + m_BaseClasses.m_size; ++i)
		{
			if (!i->m_pClass)
				continue;

			if ((i->m_pClass == other) || !strcmp(i->m_pClass->m_Name.data, other->m_Name.data))
				return true;
		}

		return false;
	}

	bool CSchemaClassInfo::UsesClass(CSchemaClassInfo* other)
	{
		if (!m_Fields.data || !m_Fields.m_size)
			return false;

		for (auto i = m_Fields.data; i != m_Fields.data + m_Fields.m_size; ++i)
		{
			if (!i->m_pType)
				continue;

			CSchemaType* schemaType = i->m_pType;

			if (!schemaType)
				continue;

			if (schemaType->GetTypeCategory() == CSchemaType::Schema_Ptr)
				continue;

			if ((std::string(schemaType->GetName())) == other->m_Name.data)
				return true;
		}

		return false;
	}

	bool CSchemaClassInfo::InheritsVirtuals()
	{
		if (!m_BaseClasses.data || !m_BaseClasses.m_size)
			return false;

		for (auto i = m_BaseClasses.data; i != m_BaseClasses.data + m_BaseClasses.m_size; ++i)
		{
			if (!i->m_pClass)
				continue;

			if (i->m_pClass->m_ClassFlags & SCHEMA_CLASS_HAS_VIRTUAL_MEMBERS || i->m_pClass->InheritsVirtuals())
				return true;
		}

		return false;
	}

	SchemaEnumeratorInfoData_t* CSchemaEnumInfo::GetFieldData(const std::string& fieldName)
	{
		if (!m_Enumerators.data || !m_Enumerators.m_size)
			return nullptr;

		for (auto i = m_Enumerators.data; i != m_Enumerators.data + m_Enumerators.m_size; ++i)
		{
			if (!i->m_Name.data)
				continue;

			if (fieldName == i->m_Name.data)
				return i;
		}

		return nullptr;
	}

	SchemaEnumeratorInfoData_t* CSchemaEnumInfo::GetFieldData(decltype(SchemaEnumeratorInfoData_t::m_nValue) fieldValue)
	{
		if (!m_Enumerators.data || !m_Enumerators.m_size)
			return nullptr;

		for (auto i = m_Enumerators.data; i != m_Enumerators.data + m_Enumerators.m_size; ++i)
		{
			if (!i->m_Name.data)
				continue;

			if (fieldValue == i->m_nValue)
				return i;
		}

		return nullptr;
	}

	std::string CSchemaType_Bitfield::TranslateToCPP(const char* memberName)
	{
		std::stringstream memberDeclaration;

		// i really hope this never happens.
		if (m_Amount == 0)
			return "";

		// bool.
		if (m_Amount == 1)
			memberDeclaration << "bool ";
		else if (m_Amount <= 8)
			memberDeclaration << "char ";
		else if (m_Amount <= 16)
			memberDeclaration << "short ";
		else if (m_Amount <= 32)
			memberDeclaration << "int ";

		memberDeclaration << memberName << " : " << m_Amount;

		return memberDeclaration.str();
	}

}

#pragma clang diagnostic pop
