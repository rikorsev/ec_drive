#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "egl_trace.h"
  
static char**            m_filtr = NULL;
static size_t            m_f_num = 0;
static egl_trace_level_t m_level = EGL_TRACE_LEVEL_DEBUG;
static egl_counter_t*    m_tstamp = NULL;

static const char* m_level_str[] =
  {
    "DEBUG",
    "INFO",
    "ERROR",
    "FAIL"
  };

void egl_trace_init(egl_trace_level_t lvl, egl_counter_t* tstamp, char** filtr, size_t num)
{
  m_level  = lvl;
  m_filtr  = filtr;
  m_f_num  = num;
  m_tstamp = tstamp;
}

static bool egl_trace_filtr_match(char* module)
{
  int i;
  if(m_filtr == NULL)
    {
      return true;
    }
  
  for(i = 0; i < m_f_num; i++)
    {
      if(strcmp(module, m_filtr[i]))
	{
	  break;
	}
    }
  return i < m_f_num ? true : false;
}

void egl_trace(egl_trace_level_t lvl, char* module, char* format, ...)
{
  va_list arg;

  assert(format);
  assert(lvl < EGL_TRACE_LEVEL_LAST);
    
  if(lvl >= m_level)
    {
      if(egl_trace_filtr_match(module) == false)
	{
	  return;
	}
      
      if(m_tstamp != NULL)
	{
	  printf("[%08u] ", egl_counter_get(m_tstamp));
	}

      if(module != NULL)
	{
	  printf("%s:", module);
	}

      printf("%s: ", m_level_str[lvl]);
      
      va_start(arg, format);
      vprintf(format, arg);
      va_end(arg);
    }
}
