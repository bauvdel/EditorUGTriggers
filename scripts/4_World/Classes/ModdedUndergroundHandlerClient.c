modded class UndergroundHandlerClient
{
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

	override protected void CalculateBreadCrumbs()
	{
		float closestDist = float.MAX;
		array<float> distances = new array<float>();
		array<float> distancesInverted = new array<float>();

		int excludeMask = 0;
		foreach (int indx, auto crumb : m_TransitionalTrigger.m_Data.Breadcrumbs)
		{
			if (indx > 32)
				break;

			float dist = vector.Distance(m_Player.GetPosition(), crumb.GetPosition());
			float crumbRadius = m_TransitionalTrigger.m_Data.Breadcrumbs[indx].Radius;
			float maxRadiusAllowed = DISTANCE_CUTOFF;

			if (crumb.ExternalValueController)
			{
				switch (crumb.ExternalValueController.Type)
				{
					case "DoorState":
					{
						BreadcrumbDoorStateController ctrl = new BreadcrumbDoorStateController(crumb.ExternalValueController.Params);
						if (!ctrl || (ctrl && ctrl.SelectionName == ""))
							break;

						Building building = Building.Cast(m_TransitionalTrigger.GetTriggerParentObject());
						if (!building)
							break;

						float animPhase = building.GetAnimationPhase(ctrl.SelectionName);
						m_LightingLerpTarget = Math.Clamp(Easing.EaseInOutQuint(1 - animPhase), 0.0, 1.0);
						m_LightingLerp = Easing.EaseInQuint(1 - Math.Remap(0.2, 0.5, 0.0, 1.0, animPhase));
						m_EyeAccoTarget = Math.Remap(0.2, 0.5, 0.0, m_TransitionalTrigger.m_Data.Breadcrumbs[indx].EyeAccommodation, animPhase * ACCO_MODIFIER);

						return;
					}
				}
			}

			if (crumbRadius != -1)
				maxRadiusAllowed = crumbRadius;
			if (dist > maxRadiusAllowed)
				excludeMask = (excludeMask | (1 << indx));
			else if (m_TransitionalTrigger.m_Data.Breadcrumbs[indx].UseRaycast)
			{
				vector rayStart;
				MiscGameplayFunctions.GetHeadBonePos(m_Player, rayStart);
				vector rayEnd = crumb.GetPosition();
				vector hitPos, hitNormal;
				float hitFraction;
				Object hitObj;

				if (DayZPhysics.RayCastBullet(rayStart, rayEnd, PhxInteractionLayers.TERRAIN | PhxInteractionLayers.ROADWAY | PhxInteractionLayers.BUILDING, null, hitObj, hitPos, hitNormal, hitFraction))
				{
					excludeMask = (excludeMask | (1 << indx));
				}
			}

			distances.Insert(dist);

			#ifdef DIAG_DEVELOPER
			if (DiagMenu.GetBool(DiagMenuIDs.UNDERGROUND_SHOW_BREADCRUMB))
				Debug.DrawSphere(crumb.GetPosition(), 0.1, COLOR_RED, ShapeFlags.ONCE);
			#endif
		}

		float baseDst = distances[0];
		float sum = 0;

		foreach (float dst : distances)
		{
			if (dst == 0)
				dst = 0.1;
			float dstInv = (baseDst / dst) * baseDst;
			sum += dstInv;
			distancesInverted.Insert(dstInv);
		}

		float sumCheck = 0;
		float eyeAcco = 0;
		foreach (int i, float dstInvert : distancesInverted)
		{
			if ((1 << i) & excludeMask)
				continue;

			float ratio = dstInvert / sum;
			if (ratio > MAX_RATIO)
				ratio = MAX_RATIO;

			if (ratio > RATIO_CUTOFF)
			{
				#ifdef DIAG_DEVELOPER
				if (DiagMenu.GetBool(DiagMenuIDs.UNDERGROUND_SHOW_BREADCRUMB))
				{
					float intensity = (1 - ratio) * 255;
					Debug.DrawLine(g_Game.GetPlayer().GetPosition() + "0 1 0", m_TransitionalTrigger.m_Data.Breadcrumbs[i].GetPosition(), ARGB(0, 255, intensity, intensity), ShapeFlags.ONCE);
				}
				#endif

				eyeAcco += ratio * m_TransitionalTrigger.m_Data.Breadcrumbs[i].EyeAccommodation;
			}
		}

		m_EyeAccoTarget = eyeAcco * ACCO_MODIFIER;
	}
}
