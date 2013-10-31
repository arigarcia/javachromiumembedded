// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "jni_util.h"
#include <jawt.h>
#include "util.h"

namespace {

JavaVM *g_jvm = NULL;

}  // namespace

void SetJVM(JavaVM* jvm) {
  ASSERT(!g_jvm);
  g_jvm = jvm;
}

JNIEnv* GetJNIEnv() {
  JNIEnv *env = NULL;
  if (g_jvm->GetEnv((void**)&env, JNI_VERSION_1_6) == JNI_EDETACHED &&
      g_jvm->AttachCurrentThread((void**)&env, NULL) != JNI_OK) {
    return NULL;
  }
  return env;
}

jobject NewJNIObject(JNIEnv* env, jclass cls) {
  jmethodID initID = env->GetMethodID(cls, "<init>", "()V");
	if(initID == 0) {
		env->ExceptionClear();
		return NULL;
	}

	jobject obj = env->NewObject(cls, initID);
	if(obj == NULL) {
		env->ExceptionClear();
		return NULL;
	}

  return obj;
}

jobject NewJNIObject(JNIEnv* env, const char* class_name) {
  jclass cls = env->FindClass(class_name);
  if (!cls)
    return NULL;

  return NewJNIObject(env, cls);
}

CefString GetJNIString(JNIEnv* env, jstring jstr)
{
	CefString cef_str;
	const char* chr = NULL;
	if(jstr)
		chr = env->GetStringUTFChars(jstr, NULL);
	if(chr)
		cef_str = chr;
	env->ReleaseStringUTFChars(jstr, chr);
	return cef_str;
}

jstring NewJNIString(JNIEnv* env, const CefString& str)
{
  std::string cstr(str);
	return env->NewStringUTF(cstr.c_str());
}

jobjectArray NewJNIStringArray(JNIEnv* env,
								               const std::vector<CefString>& vals)
{
	if(vals.empty())
		return NULL;

  jclass cls = env->FindClass("java/lang/String");
  if (!cls)
    return NULL;

	jobjectArray arr = env->NewObjectArray(
      static_cast<jsize>(vals.size()),
		  cls,
      NULL);

	for(jsize i = 0; i < vals.size(); i++)
		env->SetObjectArrayElement(arr, i, NewJNIString(env, vals[i]));

	return arr;
}

bool GetJNIFieldInt(JNIEnv* env, jclass cls, jobject obj,
                    const char* field_name, int* value) {
  jfieldID field = env->GetFieldID(cls, field_name, "I");
  if (field) {
    *value = env->GetIntField(obj, field);
    return true;
  }
  env->ExceptionClear();
  return false;
}

bool SetJNIFieldInt(JNIEnv* env, jclass cls, jobject obj,
                    const char* field_name, int value) {
  jfieldID field = env->GetFieldID(cls, field_name, "I");
  if (field) {
    env->SetIntField(obj, field, value);
    return true;
  }
  env->ExceptionClear();
  return false;
}

bool GetJNIFieldStaticInt(JNIEnv* env, jclass cls,
                          const char* field_name, int* value) {
  jfieldID field = env->GetStaticFieldID(cls, field_name, "I");
  if (field) {
    *value = env->GetStaticIntField(cls, field);
    return true;
  }
  env->ExceptionClear();
  return false;
}

bool CallJNIMethodI_V(JNIEnv* env, jclass cls, jobject obj,
                      const char* method_name, int* value) {
  jmethodID methodID = env->GetMethodID(cls, method_name, "()I");
	if (methodID) {
    *value = env->CallIntMethod(obj, methodID);
    return true;
  }
  env->ExceptionClear();
  return false;
}

bool CallJNIMethodC_V(JNIEnv* env, jclass cls, jobject obj,
                      const char* method_name, char* value) {
  jmethodID methodID = env->GetMethodID(cls, method_name, "()C");
	if (methodID) {
    *value = env->CallCharMethod(obj, methodID);
    return true;
  }
  env->ExceptionClear();
  return false;
}

CefRect GetJNIRect(JNIEnv* env, jobject obj) {
  CefRect rect;

  jclass cls = env->FindClass("java/awt/Rectangle");
  if (!cls)
    return rect;

  int x, y, width, height;
  if (GetJNIFieldInt(env, cls, obj, "x", &x) &&
      GetJNIFieldInt(env, cls, obj, "y", &y) &&
      GetJNIFieldInt(env, cls, obj, "width", &width) &&
      GetJNIFieldInt(env, cls, obj, "height", &height)) {
    rect.Set(x, y, width, height);
    return rect;
  }

  return rect;
}

jobject NewJNIRect(JNIEnv* env, const CefRect& rect) {
  jclass cls = env->FindClass("java/awt/Rectangle");
  if (!cls)
    return NULL;

  jobject obj = NewJNIObject(env, cls);
  if (!obj)
    return NULL;

  if (SetJNIFieldInt(env, cls, obj, "x", rect.x) &&
      SetJNIFieldInt(env, cls, obj, "y", rect.y) &&
      SetJNIFieldInt(env, cls, obj, "width", rect.width) &&
      SetJNIFieldInt(env, cls, obj, "height", rect.height)) {
    return obj;
  }

  env->DeleteLocalRef(obj);
  return NULL;
}

jobjectArray NewJNIRectArray(JNIEnv* env,
								             const std::vector<CefRect>& vals) {
  if(vals.empty())
		return NULL;

  jclass cls = env->FindClass("java/awt/Rectangle");
  if (!cls)
    return NULL;

	jobjectArray arr = env->NewObjectArray(
      static_cast<jsize>(vals.size()),
		  cls,
      NULL);

	for(jsize i = 0; i < vals.size(); i++)
		env->SetObjectArrayElement(arr, i, NewJNIRect(env, vals[i]));

	return arr;
}

bool GetJNIPoint(JNIEnv* env, jobject obj, int* x, int* y) {
  jclass cls = env->FindClass("java/awt/Rectangle");
  if (!cls)
    return false;

  if (GetJNIFieldInt(env, cls, obj, "x", x) &&
      GetJNIFieldInt(env, cls, obj, "y", y)) {
    return true;
  }

  return false;
}

// Create a new java.awt.Point.
jobject NewJNIPoint(JNIEnv* env, int x, int y) {
  jclass cls = env->FindClass("java/awt/Point");
  if (!cls)
    return NULL;

  jobject obj = NewJNIObject(env, cls);
  if (!obj)
    return NULL;

  if (SetJNIFieldInt(env, cls, obj, "x", x) &&
      SetJNIFieldInt(env, cls, obj, "y", y)) {
    return obj;
  }

  env->DeleteLocalRef(obj);
  return NULL;
}
