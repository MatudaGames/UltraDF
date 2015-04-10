package lv.gamevision.dwarfforest;

import android.app.Application;

public class DwarfForestApplication extends Application 
{
    private static final String APP_ID = "563836953673753";
    private static final String APP_NAMESPACE = "dwarfforestdev";
    
	@Override
	public void onCreate() {
		super.onCreate();
	}
	
	@Override
	public void onTerminate() {
		super.onTerminate();
	}
}
