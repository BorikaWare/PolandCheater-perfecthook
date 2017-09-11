#include "Chams.h"

#include "SDK.h"
#include "Interfaces.h"
#include <sstream>
#define RandomInt(nMin, nMax) (rand() % (nMax - nMin + 1) + nMin);
void InitKeyValues(KeyValues* keyValues, const char* name)
{
    static DWORD sig = U::FindPattern("client.dll", (PBYTE)"\x68\x00\x00\x00\x00\x8B\xC8\xE8\x00\x00\x00\x00\x89\x45\xFC\xEB\x07\xC7\x45\x00\x00\x00\x00\x00\x8B\x03\x56", "x????xxx????xxxxxxx?????xxx");
    sig += 7;
    sig = sig + *reinterpret_cast< PDWORD_PTR >(sig + 1) + 5;
	DWORD dwFunction = sig;
	__asm
	{
		push name
		mov ecx, keyValues
		call dwFunction
	}
}

void LoadFromBuffer(KeyValues* keyValues, char const* resourceName, const char* pBuffer)
{
    static DWORD sig  = (DWORD)U::pattern_scan(GetModuleHandleW(L"client.dll"), "55 8B EC 83 E4 F8 83 EC 34 53 8B 5D 0C 89 4C 24 04");
	DWORD dwFunction = sig;

	__asm
	{
		push 0
		push 0
		push 0
		push pBuffer
		push resourceName
		mov ecx, keyValues
		call dwFunction
	}
}

IMaterial* CreateMaterial(std::string type, std::string texture, bool ignorez, bool nofog, bool model, bool nocull, bool halflambert)
{
    static int number = 0;
    std::stringstream materialData;
    materialData << "\"" + type + "\"\n"
        "{\n"
        "\t\"$basetexture\" \"" + texture + "\"\n"
        "\t\"$ignorez\" \"" + std::to_string(ignorez) + "\"\n"
        "\t\"$envmap\" \"" + "" + "\"\n"
        "\t\"$nofog\" \"" + std::to_string(nofog) + "\"\n"
        "\t\"$model\" \"" + std::to_string(model) + "\"\n"
        "\t\"$nocull\" \"" + std::to_string(nocull) + "\"\n"
        "\t\"$selfillum\" \"" + "1" + "\"\n"
        "\t\"$halflambert\" \"" + std::to_string(halflambert) + "\"\n"
        "\t\"$znearer\" \"" + "0" + "\"\n"
        "\t\"$flat\" \"" + "1" + "\"\n"
        "}\n" << std::flush;

    std::string materialName = "material_" + std::to_string(number) + ".vmt";
    KeyValues* keyValues = new KeyValues(materialName.c_str());
    number++;
    InitKeyValues(keyValues, type.c_str());
    LoadFromBuffer(keyValues, materialName.c_str(), materialData.str().c_str());

    return g_MaterialSystem->CreateMaterial(materialName.c_str(), keyValues);
}

IMaterial* CreateMaterial(bool shouldIgnoreZ, bool isLit, bool isWireframe) 
{
	static int created = 0;

	static const char tmp[] =
	{
		"\"%s\"\
		\n{\
		\n\t\"$basetexture\" \"vgui/white\"\
		\n\t\"$envmap\" \"env_cubemap\"\
		\n\t\"$model\" \"1\"\
		\n\t\"$flat\" \"0\"\
		\n\t\"$nocull\" \"0\"\
		\n\t\"$selfillum\" \"1\"\
		\n\t\"$halflambert\" \"1\"\
		\n\t\"$nofog\" \"0\"\
		\n\t\"$ignorez\" \"%i\"\
		\n\t\"$znearer\" \"0\"\
		\n\t\"$wireframe\" \"%i\"\
        \n}\n"
	};

	char* baseType = "VertexLitGeneric";
	char material[512];
	sprintf_s(material, sizeof(material), tmp, baseType, (shouldIgnoreZ) ? 1 : 0, (isWireframe) ? 1 : 0);

	char name[512];
	sprintf_s(name, sizeof(name), "kys_%i.nerd", created);
	++created;

	KeyValues* keyValues = (KeyValues*)malloc(sizeof(KeyValues));
	InitKeyValues(keyValues, baseType);
	LoadFromBuffer(keyValues, name, material);

	IMaterial* createdMaterial = g_MaterialSystem->CreateMaterial(name, keyValues);
	createdMaterial->IncrementReferenceCount();

	return createdMaterial;
}




void ForceMaterial(Color color, IMaterial* material, bool useColor, bool forceMaterial)
{
	if (useColor)
	{
		float temp[3] =
		{
			(float)color.r(),
			(float)color.g(),
			(float)color.b()
		};

		temp[0] /= 255.f;
		temp[1] /= 255.f;
		temp[2] /= 255.f;


		float alpha = (float)color.a();

		g_RenderView->SetBlend(1.0f);
		g_RenderView->SetColorModulation(temp);
	}

	if (forceMaterial)
		g_ModelRender->ForcedMaterialOverride(material);
	else
		g_ModelRender->ForcedMaterialOverride(NULL);
}
