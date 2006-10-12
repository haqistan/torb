/* 
 * The contents of this file are subject to the Mozilla Public
 * License Version 1.1 (the "License"); you may not use this file
 * except in compliance with the License. You may obtain a copy of
 * the License at http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS
 * IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
 * implied. See the License for the specific language governing
 * rights and limitations under the License.
 *
 * The Original Code is mozilla.org code.
 *
 * The Initial Developer of the Original Code is Sun Microsystems,
 * Inc. Portions created by Sun are
 * Copyright (C) 1999 Sun Microsystems, Inc. All
 * Rights Reserved.
 *
 * Contributor(s): 
 */
#include"PlugletsDir.h"
#include "prenv.h"
#include "PlugletLog.h"
#include "nsFileSpec.h"
#include "nsDirectoryServiceDefs.h"
#include "nsIDirectoryService.h"

PlugletsDir::PlugletsDir(void) {
    list = NULL;
    //nb ???
}

PlugletsDir::~PlugletsDir(void) {
    if (list) {
	for (ListIterator *iter = list->GetIterator();iter->Get();delete iter->Get(),iter->Next())
	    ;
	delete list;
    }
}

void PlugletsDir::LoadPluglets() {
    PR_LOG(PlugletLog::log, PR_LOG_DEBUG,
	    ("PlugletsDir::LoadPluglets\n"));
    if (!list) {
	list = new List();
	char * path = PR_GetEnv("PLUGLET");
	if (!path) {
	  nsCOMPtr<nsIFile> sysDir;
	  nsresult rv = NS_GetSpecialDirectory(NS_OS_CURRENT_PROCESS_DIR,
					       getter_AddRefs(sysDir));
          if (NS_FAILED(rv))
	    return;

	  rv = sysDir->AppendNative(NS_LITERAL_CSTRING("plugins"));
	  if (NS_FAILED(rv))
	    return;

	  nsXPIDLCString pluginsDir;
	  rv = sysDir->GetNativePath(pluginsDir);
	  if (NS_FAILED(rv))
	    return;

	  path = PL_strdup(pluginsDir.get());
	}
	if (!path) {
	    return;
	}
	PlugletFactory *plugletFactory;
	nsFileSpec dir(path);
	for (nsDirectoryIterator iter(dir,PR_TRUE); iter.Exists(); iter++) {
	    const nsFileSpec& file = iter;
	    const char* name = file.GetCString();
	    int length;
	    if((length = strlen(name)) <= 4  // if it's shorter than ".jar"
	       || strcmp(name+length - 4,".jar") ) {
		continue;
	    }
	    if ( (plugletFactory = PlugletFactory::Load(name)) ) {
		list->Add(plugletFactory);
	    }
	}
    }
}

nsresult PlugletsDir::GetPlugletFactory(const char * mimeType, PlugletFactory **plugletFactory) {
    if(!plugletFactory) {
	return NS_ERROR_NULL_POINTER;
    }
    *plugletFactory = NULL;
    nsresult res = NS_ERROR_FAILURE;
    if(!list) {
	LoadPluglets();
    }
    for (ListIterator *iter = list->GetIterator();iter->Get();iter->Next()) {
	if(((PlugletFactory*)(iter->Get()))->Compare(mimeType)) {
	    *plugletFactory = (PlugletFactory*)iter->Get();
	    res = NS_OK;
	    break;
	}
    }
    return res;
}













