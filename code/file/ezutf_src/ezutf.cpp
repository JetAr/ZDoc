// ezutf.cpp
// Copyright (c) 2007, AlpineSoft

#include "stdafx.h"
#include "hpsutils.h"
#include "textfile.h"


int _tmain (int argc, _TCHAR* argv[])
{
    TextFile *tf_in = Hnew TextFile;
    int result = tf_in->Open (__T ("ezutf_test_input.txt"), TF_READ);
    if (result < 0)
    {
        TCHAR *errmsg = GetLastErrorString ();
        _tprintf (__T ("%s\n"), errmsg);
        delete tf_in;
        return 1;
    }

    DWORD t0 = GetTickCount ();
    int n_lines = 0;
    TCHAR *line_buf = NULL;

    for ( ; ; )
    {
        result = tf_in->ReadLine (NULL, &line_buf);
        if (result < 0)
        {
            if (result == TF_ERROR)                 // else TF_EOF
            {
                TCHAR *errmsg = GetLastErrorString ();
                _tprintf (__T ("%s\n"), errmsg);
            }
            break;
        }
        
        ++n_lines;
    }

    if (result >= TF_EOF)
    {
        DWORD t = (GetTickCount () - t0 + 500) / 1000;
        if (t == 0)
            t = 1;
        _tprintf (__T ("Read %d lines in %d seconds (%d lines per second)\n"), 
            n_lines, t, n_lines / t);
    }

    tf_in->Close ();
    result = tf_in->Open (__T ("ezutf_test_input.txt"), TF_READ);
    if (result < 0)
    {
        TCHAR *errmsg = GetLastErrorString ();
        _tprintf (__T ("%s\n"), errmsg);
        delete tf_in;
        return 1;
    }

    int encoding = tf_in->GetEncoding ();
    TextFile *tf_out = Hnew TextFile;
    result = tf_out->Open (__T ("ezutf_test_output.txt"), TF_WRITE, encoding);
    if (result < 0)
    {
        TCHAR *errmsg = GetLastErrorString ();
        _tprintf (__T ("%s\n"), errmsg);
        delete tf_in;
        delete tf_out;
        return 1;
    }

    t0 = GetTickCount ();
    n_lines = 0;

    for ( ; ; )
    {
        result = tf_in->ReadLine (NULL, &line_buf);
        if (result < 0)
        {
            if (result == TF_ERROR)                 // else TF_EOF
            {
                TCHAR *errmsg = GetLastErrorString ();
                _tprintf (__T ("%s\n"), errmsg);
            }
            break;
        }

        result = tf_out->WriteString (NULL, line_buf);
        if (result >= 0)
            result = tf_out->WriteChar (NULL, '\n');
        if (result < 0)
        {
            if (result == TF_ERROR)                 // else TF_EOF
            {
                TCHAR *errmsg = GetLastErrorString ();
                _tprintf (__T ("%s\n"), errmsg);
            }
            break;
        }

        ++n_lines;
    }

    free_block (line_buf);
    if (result >= TF_EOF)
    {
        DWORD t = (GetTickCount () - t0 + 500) / 1000;
        if (t == 0)
            t = 1;
        _tprintf (__T ("%d lines copied in %d seconds (%d lines per second)\n"), 
            n_lines, t, n_lines / t);
    }

    delete tf_in;
    delete tf_out;

    return 0;
}

