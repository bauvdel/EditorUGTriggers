// Vanilla cfgundergroundtriggers.json fields the editor doesn't expose for
// editing but must carry through import -> placed objects -> export unchanged,
// so round-tripping a vanilla file doesn't silently lose data:
//   - Comment (trigger + breadcrumb): added in DayZ 1.30; vanilla labels every
//     Badlands (nasdara) trigger/breadcrumb with it.
//   - CustomSpawn / Tag / ParentNetworkId (trigger): attach a trigger to a map
//     building or ObjectSpawner object (used by Sakhal's bunkers).
//   - ExternalValueController (breadcrumb): e.g. {"Type":"DoorState",
//     "Params":["maindoor2_outer"]} drives darkness from a door's state.
// Null/empty values are kept as-is so absent keys stay absent on export.

class UGTriggerMeta
{
	string Comment;
	bool CustomSpawn;
	string Tag;
	ref TIntArray ParentNetworkId;

	UGTriggerMeta Copy()
	{
		UGTriggerMeta m = new UGTriggerMeta();
		m.Comment = Comment;
		m.CustomSpawn = CustomSpawn;
		m.Tag = Tag;
		m.ParentNetworkId = UG_CopyIntArray(ParentNetworkId);
		return m;
	}

	void ReadFrom(JsonUndergroundAreaTriggerData d)
	{
		if (!d) return;
		Comment = d.Comment;
		CustomSpawn = d.CustomSpawn;
		Tag = d.Tag;
		ParentNetworkId = UG_CopyIntArray(d.ParentNetworkId);
	}

	void ApplyTo(JsonUndergroundAreaTriggerData d)
	{
		if (!d) return;
		d.Comment = Comment;
		d.CustomSpawn = CustomSpawn;
		d.Tag = Tag;
		d.ParentNetworkId = UG_CopyIntArray(ParentNetworkId);
	}
}

class UGBreadcrumbMeta
{
	string Comment;
	ref BreadcrumbExternalValueController ExternalValueController;

	UGBreadcrumbMeta Copy()
	{
		UGBreadcrumbMeta m = new UGBreadcrumbMeta();
		m.Comment = Comment;
		m.ExternalValueController = UG_CopyEVC(ExternalValueController);
		return m;
	}

	void ApplyTo(JsonUndergroundAreaBreadcrumb b)
	{
		if (!b) return;
		b.Comment = Comment;
		b.ExternalValueController = UG_CopyEVC(ExternalValueController);
	}
}

TIntArray UG_CopyIntArray(TIntArray src)
{
	if (!src) return null;
	TIntArray dst = new TIntArray();
	dst.Copy(src);
	return dst;
}

BreadcrumbExternalValueController UG_CopyEVC(BreadcrumbExternalValueController src)
{
	if (!src) return null;
	BreadcrumbExternalValueController dst = new BreadcrumbExternalValueController();
	dst.Type = src.Type;
	if (src.Params)
	{
		dst.Params = new TStringArray();
		dst.Params.Copy(src.Params);
	}
	return dst;
}
