/****************************************************************************
Copyright (c) 2010-2011 cocos2d-x.org

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
 ****************************************************************************/
package lv.gamevision.dwarfforest;

import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;

import org.cocos2dx.lib.Cocos2dxActivity;
import org.cocos2dx.lib.Cocos2dxGLSurfaceView;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.Signature;
import android.content.pm.PackageManager.NameNotFoundException;
import android.net.Uri;
import android.os.Bundle;
import android.os.Vibrator;
import android.util.Base64;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;

import android.R.string;

import com.flurry.android.FlurryAgent;
//import com.flurry.android.monolithic.sdk.impl.ms;
import com.diwublog.AnalyticX.*;

import com.google.android.gms.common.GooglePlayServicesUtil;
import com.google.android.gms.games.achievement.AchievementBuffer;

//import com.testflightapp.lib.TestFlight;

public class DwarfForest extends BaseGameActivity {
	static Context mContext;
	private static DwarfForest instance = null;

	public DwarfForest() {
		super();
		instance = this;
	}

	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		mContext = DwarfForest.this;

		// AnalyticXBridge.sessionContext = this.getApplicationContext();
		// FlurryAgent.onStartSession(this, "3VKQNCDJQW6Q9YHVYVKH");

		AnalyticXBridge.sessionContext = this.getApplicationContext();
	}
	
	public Cocos2dxGLSurfaceView onCreateView() { 
		Cocos2dxGLSurfaceView glSurfaceView = new Cocos2dxGLSurfaceView(this); // DwarfForest should create
		 //stencil buffer // glSurfaceView.setEGLConfigChooser(5, 6, 5, 0, 16, 8);
		 glSurfaceView.setEGLConfigChooser(8 , 8, 8, 8, 16, 0);
//	     glSurfaceView.setEGLConfigChooser(5, 6, 5, 0, 16, 8);
	     return glSurfaceView; 
	}

	static {
		// System.loadLibrary("dwarfforest");
		System.loadLibrary("cocos2dcpp");
	}

	@Override
	public void onPause() { 
		super.onPause();
	}

	@Override
	public void onResume() {
		super.onResume();
	}

	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		super.onActivityResult(requestCode, resultCode, data);
	}

	public void openLeaderboard() {
		System.out.println("OPENZZZ");
	}

	public static void getTest() {
		System.out.println("TESTZZZZZ");
	}

	public static DwarfForest getInstance() {
		System.out.println("INSTANCE TESTZZZZZ");
		return instance;
	}

	public static void vibrate() {
		/*
		// Get instance of Vibrator from current Context
		Vibrator v = (Vibrator) getContext().getSystemService(
				Context.VIBRATOR_SERVICE);
		v.vibrate(300);
		*/
	}

	// ////////////////////

	public static void gameServicesSignIn() {
		((DwarfForest) mContext).runOnUiThread(new Runnable() {
			public void run() {
				((DwarfForest) mContext).beginUserInitiatedSignIn();
			}
		});
	}

	public static void gameServicesSignOut() {
		((DwarfForest) mContext).runOnUiThread(new Runnable() {
			public void run() {
				((DwarfForest) mContext).signOut();
			}
		});
	}

	public static int gameServiceIsSigned() {
		Boolean isInside = false;
		isInside = ((DwarfForest) mContext).isSignedIn();
		if (isInside)
			return 1;
		return 0;
	}

	public static void updateTopScoreLeaderboard(int score) {
		((DwarfForest) mContext).getGamesClient().submitScore("leaderboardid",
				score);
	}

	public static void updateAchievement(String id, int percentage) {
		((DwarfForest) mContext).getGamesClient().incrementAchievement(id,
				percentage);
	}

	public static void showTwitterFollow() {
		Intent browserIntent = new Intent(Intent.ACTION_VIEW,
				Uri.parse("http://mobile.twitter.com/GameVision_lv"));
		((DwarfForest) mContext).startActivity(browserIntent);
	}

	public static void showFacebookFollow() {
		Intent browserIntent = new Intent(Intent.ACTION_VIEW,
				Uri.parse("https://www.facebook.com/GameVisionLV"));
		((DwarfForest) mContext).startActivity(browserIntent);
	}

	public static void openURL(String url) {
		Intent i = new Intent(Intent.ACTION_VIEW);
		i.setData(Uri.parse(url));
		((DwarfForest) mContext).startActivity(i);
	}

	public static void showLeaderboards() {

		// Intent twit = new Intent(Intent.ACTION_VIEW);
		// twit.setData(Uri.parse("http://twitter.com/matudagames"));
		// mContext.startActivity(twit);

		// Intent browserIntent = new Intent(Intent.ACTION_VIEW,
		// Uri.parse("http://twitter.com/matudagames"));
		// ((DwarfForest)mContext).startActivity(browserIntent);

		// ((DwarfForest)mContext).startActivity(twit);

		if (!((DwarfForest) mContext).isSignedIn()) {
			((DwarfForest) mContext).runOnUiThread(new Runnable() {
				public void run() {
					((DwarfForest) mContext)
							.showAlert("Please sign in to view leaderboards.");
				}
			});
			return;
		}

		((DwarfForest) mContext).runOnUiThread(new Runnable() {
			public void run() {
				// startActivityForResult(getGamesClient().getAllLeaderboardsIntent(),
				// RC_UNUSED);//To show all leaderborados
				((DwarfForest) mContext).startActivityForResult(
						((DwarfForest) mContext)
								.getGamesClient()
								.getLeaderboardIntent(
										mContext.getString(R.string.leaderboard_easy)),
						5001);
			}
		});
	}

	public native static void onLoginSuccess();

	public native static void onRecievedSign();

	public static void showAchievements() {
		if (!((DwarfForest) mContext).isSignedIn()) {
			((DwarfForest) mContext).runOnUiThread(new Runnable() {
				public void run() {
					((DwarfForest) mContext)
							.showAlert("Please sign in to view achievements.");
				}
			});
			return;
		}

		((DwarfForest) mContext).runOnUiThread(new Runnable() {
			public void run() {
				((DwarfForest) mContext).startActivityForResult(
						((DwarfForest) mContext).getGamesClient()
								.getAchievementsIntent(), 5001);
			}
		});
	}

	// Try to complete the achievementos
	public static void completeAchievment(String theAchievment, int amount) {
		System.out.println(theAchievment);
		if (!((DwarfForest) mContext).isSignedIn()) {
			return;
		}

		String packageName = getContext().getPackageName();
		int resId = getContext().getResources().getIdentifier(theAchievment,
				"string", packageName);
		String achievmentID = getContext().getString(resId);

		if (amount > 0) {
			((DwarfForest) mContext).getGamesClient().incrementAchievement(
					achievmentID, 1);
		} else {
			((DwarfForest) mContext).getGamesClient().unlockAchievement(
					achievmentID);
		}
	}

	public static void submitScoreToLeaderboard(String theLeaderboard,
			int aScore) {
		if (!((DwarfForest) mContext).isSignedIn()) {
			return;
		}

		String packageName = getContext().getPackageName();
		int resId = getContext().getResources().getIdentifier(theLeaderboard,
				"string", packageName);
		String leaderboardID = getContext().getString(resId);

		((DwarfForest) mContext).getGamesClient().submitScore(leaderboardID,
				aScore);
	}

	// public boolean onKeyDown(int keyCode, KeyEvent event) {
	// // exit program when key back is entered
	// if (keyCode == KeyEvent.KEYCODE_BACK) {
	// android.os.Process.killProcess(android.os.Process.myPid());
	// }
	// return super.onKeyDown(keyCode, event);
	// }

	@Override
	public void onSignInFailed() {
		onRecievedSign();
		// What is the error ???
		int aCode = GooglePlayServicesUtil
				.isGooglePlayServicesAvailable((DwarfForest) mContext);
		String testString = Integer.toString(aCode);
		Log.v("LOGIN ERROR", testString);
		Log.v(mDebugTag, "onSignInFailed");
	}

	@Override
	public void onSignInSucceeded() {
		onLoginSuccess();
		Log.v(mDebugTag, "onSignInSucceeded");
	}

	// Save the achievement stuff !!!

	// ////////

	@Override
	protected void onStart() {
		super.onStart();
		FlurryAgent.onStartSession(this, "2ZCVWPY7VCRNSVQTWJJB");

		// TestFlight.takeOff(this, "813bfc02-7490-4c21-a5d0-417982ba6c0e");
	}

	@Override
	protected void onStop() {
		super.onStop();
		FlurryAgent.onEndSession(this);
	}

	public void submitScore(double points, int level) {
		System.out.println("Call for submitScore");
		this.runOnUiThread(new SubmitScoreRunnable(points, level));
	}
}

class SubmitScoreRunnable implements Runnable {
	private double points;
	private int level;

	SubmitScoreRunnable(double points, int level) {

		this.points = points;
		this.level = level;
	}

	@Override
	public void run() {
		System.out.println("Submiting for submitScore");
		System.out.println(points);
		System.out.println(level);

	}
}

/*
 * import org.cocos2dx.lib.Cocos2dxActivity; import
 * org.cocos2dx.lib.Cocos2dxGLSurfaceView;
 * 
 * import android.os.Bundle;
 * 
 * public class DwarfForest extends Cocos2dxActivity{
 * 
 * protected void onCreate(Bundle savedInstanceState){
 * super.onCreate(savedInstanceState); }
 * 
 * public Cocos2dxGLSurfaceView onCreateView() { Cocos2dxGLSurfaceView
 * glSurfaceView = new Cocos2dxGLSurfaceView(this); // DwarfForest should create
 * stencil buffer // glSurfaceView.setEGLConfigChooser(5, 6, 5, 0, 16, 8);
 * glSurfaceView.setEGLConfigChooser(8 , 8, 8, 8, 16, 0);
 * 
 * return glSurfaceView; }
 * 
 * static { System.loadLibrary("cocos2dcpp"); } }
 */
