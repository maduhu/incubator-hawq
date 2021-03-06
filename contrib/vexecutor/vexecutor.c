/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */


#include "vexecutor.h"
#include "utils/guc.h"

PG_MODULE_MAGIC;

/*
 * hook function
 */
static PlanState* VExecInitNode(Plan *node,EState *eState,int eflags);
static TupleTableSlot* VExecProcNode(PlanState *node);
static bool VExecEndNode(PlanState *node);

/*
 * _PG_init is called when the module is loaded. In this function we save the
 * previous utility hook, and then install our hook to pre-intercept calls to
 * the copy command.
 */
void
_PG_init(void)
{
	elog(DEBUG3, "PG INIT VEXECTOR");
	vmthd.ExecInitNode_Hook = VExecInitNode;
	vmthd.ExecProcNode_Hook = VExecProcNode;
	vmthd.ExecEndNode_Hook = VExecEndNode;
	DefineCustomBoolVariable("vectorized_executor_enable",
	                         gettext_noop("enable vectorized executor"),
	                         NULL,
	                         &vmthd.vectorized_executor_enable,
	                         PGC_USERSET,
	                         NULL,NULL);
}

/*
 * _PG_fini is called when the module is unloaded. This function uninstalls the
 * extension's hooks.
 */
void
_PG_fini(void)
{
	elog(DEBUG3, "PG FINI VEXECTOR");
	vmthd.ExecInitNode_Hook = NULL;
	vmthd.ExecProcNode_Hook = NULL;
	vmthd.ExecEndNode_Hook = NULL;
}

static PlanState* VExecInitNode(Plan *node,EState *eState,int eflags)
{
	elog(DEBUG3, "PG VEXEC INIT NODE");
	return NULL;
}
static TupleTableSlot* VExecProcNode(PlanState *node)
{
	return NULL;
}

static bool VExecEndNode(PlanState *node)
{
	elog(DEBUG3, "PG VEXEC END NODE");
	return false;
}
