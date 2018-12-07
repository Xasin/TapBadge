
package com.xasin;

import android.content.Context;
import android.content.Intent;
import android.util.Log;

import org.qtproject.qt5.android.bindings.QtService;

public class ServiceStarter extends QtService {
	public static void startMyService(Context ctx) {
		ctx.startService(new Intent(ctx, ServiceStarter.class));
		Log.i("Service", "Something tried me!");
	}
}
