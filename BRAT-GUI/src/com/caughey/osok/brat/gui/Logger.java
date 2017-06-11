package com.caughey.osok.brat.gui;

import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.Date;

public class Logger {
	
	private String className;
	private SimpleDateFormat dateFormater = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss.SSSS");
	private StringBuffer buff = new StringBuffer();
	private static FileWriter outputFile;
	
	
	public Logger(String className){
		this.className = className;
	}
	
	public void debug(String output){
		if(Config.getInstance().logLevelDebug()){
			writeOut(output);
		}
	}
	public void info(String output){
		if(Config.getInstance().logLevelInfo() ||Config.getInstance().logLevelDebug() ){
			writeOut(output);
		}
		
	}
	public void warn(String output){
		if(Config.getInstance().logLevelWarn() ||Config.getInstance().logLevelInfo() ||Config.getInstance().logLevelDebug() ){
			writeOut(output);
		}
		
	}
	public void error(String output){
		writeOut(output);
	}
	
	private void writeOut(String output){
//		if(outputFile==null){
//			try {
//				outputFile = new FileWriter("brat-gui.log");
//			} catch (IOException e) {
//				e.printStackTrace();
//			}
//		}
		buff.delete(0,  buff.length());
		buff.append(dateFormater.format(new Date()));
		buff.append(" ");
		buff.append(className);
		buff.append(" ");
		buff.append(output);
		System.out.println(buff.toString());
//		try {
//			outputFile.write(buff.toString() + "\n");
//			outputFile.flush();
//		} catch (IOException e) {
//			e.printStackTrace();
//		}
		
	}

}
