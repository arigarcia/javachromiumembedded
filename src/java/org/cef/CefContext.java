// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

package org.cef;

/**
 * Exposes static methods for managing the global CEF context.
 */
public class CefContext {
  /**
   * Initialize the context.
   * @return true on success
   */
  public static final boolean initialize(String cachePath) {
    System.out.println("initialize on " + Thread.currentThread());
    try {
      System.loadLibrary("icudt");
      System.loadLibrary("libcef");
      System.loadLibrary("jcef");
      return N_Initialize(System.getProperty("java.library.path"), cachePath);
    } catch (UnsatisfiedLinkError err) {
      err.printStackTrace();
    }
    return false;
  }

  /**
   * Shut down the context.
   */
  public static final void shutdown() {
    System.out.println("shutdown on " + Thread.currentThread());
    System.runFinalization();
    try {
      N_Shutdown();
    } catch (UnsatisfiedLinkError err) {
      err.printStackTrace();
    }
  }

  /**
   * Perform a single message loop iteration.
   */
  public static final void doMessageLoopWork() {
    try {
      N_DoMessageLoopWork();
    } catch (UnsatisfiedLinkError err) {
      err.printStackTrace();
    }
  }

  /**
   * Create a new browser.
   */
  public static final CefBrowser createBrowser(CefHandler handler, long windowHandle, String url, boolean transparent) {
    try {
      return N_CreateBrowser(handler, windowHandle, url, transparent);
    } catch (UnsatisfiedLinkError err) {
      err.printStackTrace();
    }
    return null;
  }
  
  /**
   * Returns the native window handle for the specified native surface handle.
   */
  public static final long getWindowHandle(long surfaceHandle) {
    try {
      return N_GetWindowHandle(surfaceHandle);
    } catch (UnsatisfiedLinkError err) {
      err.printStackTrace();
    }
    return 0;
  }

  private static final native boolean N_Initialize(String pathToJavaDLL, String cachePath);
  private static final native void N_Shutdown();
  private static final native void N_DoMessageLoopWork();
  private static final native CefBrowser N_CreateBrowser(CefHandler handler, long windowHandle, String url, boolean transparent);
  private static final native long N_GetWindowHandle(long surfaceHandle);
}
