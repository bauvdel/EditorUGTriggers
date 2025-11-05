modded class UndergroundHandlerClient
{
	// Override StopAmbientSound to fix sound not stopping in editor
	override protected void StopAmbientSound()
	{
		if (m_AmbientController != string.Empty)
		{
			SetSoundControllerOverride(m_AmbientController, 0, SoundControllerAction.None);
			m_AmbientController = string.Empty;
		}
		else if (m_AmbientSound)
		{
			m_AmbientSound.SetSoundFadeOut(0);
			m_AmbientSound.SoundStop();
			m_Player.StopSoundSet(m_AmbientSound);
		}
	}
}
