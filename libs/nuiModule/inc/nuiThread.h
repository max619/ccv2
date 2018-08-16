/** 
 * \file      nuiThread.h
 *            
 * Basic abstraction of thread/semaphore for WIN32 and Posix
 *            
 * Some parts Copyright (C) 2010 Movid Authors.  All rights reserved.
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 * 
 * \author    Scott Halstvedt
 * \author    Anatoly Lushnikov
 * \author    Maxim Bagryantsev
 * \date      2012-2018
 * \copyright Copyright 2012 NUI Group. All rights reserved.
 * \copyright Copyright 2018 Appollo Pro. All rights reserved.
 */


#ifndef NUI_THREAD_H
#define NUI_THREAD_H

#ifndef PT_PASYNC
#define PT_PASYNC

#include "pasync.h"

#endif


class nuiThread;

typedef void (*thread_process_t)(nuiThread *thread);

class nuiThread : public pt::thread 
{
public:
    nuiThread(thread_process_t _process, void *_userdata);
    virtual ~nuiThread();
    void *getUserData();
    void execute();
    void stop();
    void wait();
    void post();
    bool wantQuit();
    bool relax(int);
private:
    thread_process_t process;
    pt::trigger *thread_trigger;
    void *userdata;
    bool want_quit;
};

#endif
