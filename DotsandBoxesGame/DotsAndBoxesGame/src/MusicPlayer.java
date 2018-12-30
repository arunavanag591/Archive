/////////////////////////////////////////////
////    THIS IS YOUR MUSIC PLAYER        ////
/////////////////////////////////////////////

import java.io.IOException;
import java.net.URL;

import javax.sound.sampled.AudioInputStream;
import javax.sound.sampled.AudioSystem;
import javax.sound.sampled.Clip;
import javax.sound.sampled.LineUnavailableException;
import javax.sound.sampled.UnsupportedAudioFileException;
import javazoom.jl.player.Player;


public class MusicPlayer implements Runnable{

	
	public MusicPlayer()
	{
		Thread t = new Thread(this);
		t.start();
	}
	@Override
	public void run() {
				
		//music
		 try {
	         // Open an audio input stream. You can use any kind of music just add the file 
			 //to the source and type the name but it has some file
			 //support issues. To play mp3 and other file systems you need to import different jar files
			   AudioInputStream audioIn =AudioSystem.getAudioInputStream(this.getClass().getResource("try.wav"));
	         // Get a sound clip resource.
	         Clip clip = AudioSystem.getClip();
	         // Open audio clip and load samples from the audio input stream.
	         clip.open(audioIn);
	         //clip.start();
	         clip.loop(300);
	      } catch (UnsupportedAudioFileException e) {
	         e.printStackTrace();
	      } catch (IOException e) {
	         e.printStackTrace();
	      } catch (LineUnavailableException e) {
	         e.printStackTrace();
	      }
	}

}
