﻿// SaveDOM.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <msxml6.h>

#pragma warning(disable : 4127)  // conditional expression is constant
// Macro that calls a COM method returning HRESULT value.
#define CHK_HR(stmt)        do{ hr=(stmt); if (FAILED(hr)) goto CleanUp; } while(0)

// Macro to verify memory allcation.
#define CHK_ALLOC(p)        do { if (!(p)) { hr = E_OUTOFMEMORY; goto CleanUp; } } while(0)

// Macro that releases a COM object if not NULL.
#define SAFE_RELEASE(p)     do { if ((p)) { (p)->Release(); (p) = NULL; } } while(0)

// Helper function to create a DOM instance.
HRESULT CreateAndInitDOM(IXMLDOMDocument **ppDoc)
{
    HRESULT hr = CoCreateInstance(__uuidof(DOMDocument60), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(ppDoc));
    if (SUCCEEDED(hr))
    {
        // these methods should not fail so don't inspect result
        (*ppDoc)->put_async(VARIANT_FALSE);
        (*ppDoc)->put_validateOnParse(VARIANT_FALSE);
        (*ppDoc)->put_resolveExternals(VARIANT_FALSE);
    }
    return hr;
}

// Helper function to create a VT_BSTR variant from a null terminated string.
HRESULT VariantFromString(PCWSTR wszValue, VARIANT &Variant)
{
    HRESULT hr = S_OK;
    BSTR bstrString = SysAllocString(wszValue);
    CHK_ALLOC(bstrString);

    V_VT(&Variant)   = VT_BSTR;
    V_BSTR(&Variant) = bstrString;

CleanUp:
    return hr;
}

HRESULT saveDOM()
{
    HRESULT hr = S_OK;
    IXMLDOMDocument *pXMLDom=NULL;
    IXMLDOMParseError *pXMLErr = NULL;
    BSTR bstrXML = NULL;
    BSTR bstrErr = NULL;
    VARIANT_BOOL varStatus;
    VARIANT varFileName;

    VariantInit(&varFileName);

    CHK_HR(CreateAndInitDOM(&pXMLDom));

    bstrXML = SysAllocString(L"<r>\n<t>top</t>\n<b>bottom</b>\n</r>");
    CHK_ALLOC(bstrXML);
    CHK_HR(pXMLDom->loadXML(bstrXML, &varStatus));

    if (varStatus == VARIANT_TRUE)
    {
        CHK_HR(pXMLDom->get_xml(&bstrXML));
        wprintf(L"XML DOM loaded from app:\n%s\n", bstrXML);

        VariantFromString(L"myData.xml", varFileName);
        CHK_HR(pXMLDom->save(varFileName));
        wprintf(L"XML DOM saved to myData.xml\n");
    }
    else
    {
        // Failed to load xml, get last parsing error
        CHK_HR(pXMLDom->get_parseError(&pXMLErr));
        CHK_HR(pXMLErr->get_reason(&bstrErr));
        wprintf(L"Failed to load DOM from xml string. %s\n", bstrErr);
    }

CleanUp:
    SAFE_RELEASE(pXMLDom);
    SAFE_RELEASE(pXMLErr);
    SysFreeString(bstrXML);
    SysFreeString(bstrErr);
    VariantClear(&varFileName);

    return hr;
}

int __cdecl wmain()
{
    HRESULT hr = CoInitialize(NULL);
    if(SUCCEEDED(hr))
    {
        hr = saveDOM();
        if (FAILED(hr))
        {
            wprintf(L"Failed.\n");
        }
        CoUninitialize();
    }

    return 0;

}
