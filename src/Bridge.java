package windui;

import android.view.MotionEvent;
import android.view.View;
import android.webkit.WebView;
import android.webkit.WebViewClient;

/**
 * Tiny UI-thread / input dispatch bridge. This file is represented in the
 * shipped .so by a DEX 035 generated from tools/make_bridge_dex.py.
 * Keep method signatures identical to src/loader.cpp RegisterNatives.
 *
 * Input, rendering and the mod API live in the C++ loader and windui.html.
 * No Minecraft gameplay methods are called or patched.
 */
public final class Bridge extends WebViewClient implements Runnable,
        View.OnClickListener, View.OnTouchListener {
    public final int id;

    public Bridge(int id) {
        super();
        this.id = id;
    }

    public static native void nativeRun(int id);
    public static native void nativeClick(int id, View view);
    public static native boolean nativeTouch(int id, View view, MotionEvent event);
    public static native boolean nativeUrl(int id, WebView view, String url);

    @Override public void run() { nativeRun(id); }
    @Override public void onClick(View view) { nativeClick(id, view); }
    @Override public boolean onTouch(View view, MotionEvent event) {
        return nativeTouch(id, view, event);
    }
    @Override public boolean shouldOverrideUrlLoading(WebView view, String url) {
        return nativeUrl(id, view, url);
    }
}
