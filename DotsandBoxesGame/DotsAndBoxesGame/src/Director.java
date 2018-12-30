import java.io.IOException;
import java.net.URL;
import java.io.BufferedInputStream;
import java.io.FileInputStream;

import javazoom.jl.player.Player;

import javax.sound.sampled.AudioInputStream;
import javax.sound.sampled.AudioSystem;
import javax.sound.sampled.Clip;
import javax.sound.sampled.LineUnavailableException;
import javax.sound.sampled.UnsupportedAudioFileException;

/*as created on 04/26/2015-BlackMamba591*/
public class Director {

	
	private GameSplash gs;
	private int gameType;   // 1 is pvp, 2 is pvea, 3 is pvha
	private GameFrame gf;
	
	public static void main(String[] args)
	{
		Director d = new Director();
		
	}
	
	public Director()
	{
		MusicPlayer mp = new MusicPlayer();
		 
		 this.gameType = 0;
		 this.gs = new GameSplash(this);
		 this.gs.setVisible(true);
		 
		 		
	}
	
	public void launchGame()
	{
		 if(this.gameType == 1)
		 {
			 System.out.println("PVP GAME");
			 this.gf = new GameFrame();
			 this.gs.dispose();
			 
		 }
		 if(this.gameType == 2)
		 {
			 System.out.println("PVEA GAME");
			 this.gf = new GameFrame(2);
			 this.gs.dispose();
			
			 
		 }
		 if(this.gameType == 3)
		 {
			 System.out.println("PVHA GAME");
			 this.gf = new GameFrame(3);
			 this.gs.dispose();
			
			 
		 }
	}
	
	public void setGameType(int i)
	{
		this.gameType = i;
	}
}
