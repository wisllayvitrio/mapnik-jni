package mapnik;

import java.util.ArrayList;

public class Grid extends NativeObject {
	//mapnik::grid
	
	private static native long alloc(int width, int height, String key, ArrayList<String> fields, int resolution);
	private static native long alloc(Grid other);
	native void dealloc(long ptr);
	
	public Grid(int width, int height, String key, ArrayList<String> fields, int resolution) {
		ptr=alloc(width, height, key, fields, resolution);
	}
	
	public Grid(int width, int height, String key, ArrayList<String> fields) {
		this(width, height, key, fields, 1);
	}
	
	public Grid(Grid other) {
		ptr=alloc(other);
	}
	
	public native int getWidth();
	public native int getHeight();

	public native String saveToMemory(String layername);
}
