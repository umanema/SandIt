using Godot;
using System;




public partial class Main : Node
{
	int counter = 0;
	Variant simulation;
	Vector2[] outline;
	Vector2[] outline_simplified;

	PackedScene sandPileCollision = GD.Load<PackedScene>("res://objects/sand_pile_collision.tscn");
	Node sandPileCollisionInstance;

	// Called when the node enters the scene tree for the first time.
	public override void _Ready()
	{
		simulation = ClassDB.Instantiate("GranularSimulation");
		sandPileCollisionInstance = sandPileCollision.Instantiate();
		AddChild(sandPileCollisionInstance);
	}

	// Called every frame. 'delta' is the elapsed time since the previous frame.
	public override void _Process(double delta)
	{

		Step(1);
		outline = simulation.AsGodotObject().Call("get_outline").AsVector2Array();
		outline_simplified = simulation.AsGodotObject().Call("get_simplified_outline").AsVector2Array();

		sandPileCollisionInstance.GetNode<Line2D>("Outline_Pile").Points = MapOutline(outline);
		sandPileCollisionInstance.GetNode<Line2D>("Outline_Collision").Points = MapOutline(outline_simplified);
		sandPileCollisionInstance.GetNode<CollisionPolygon2D>("StaticBody/CollisionPolygon2D").Polygon = MapOutline(outline_simplified);
	}

	Vector2[] MapOutline(Vector2[] outline) {
		Vector2[] mappedOutline = new Vector2[outline.Length];
		int i = 0;
		foreach (Vector2 position in outline) {
			mappedOutline[i] = new Vector2(position.Y * Common.pixelScale, position.X * Common.pixelScale);
			i++;
		}
		return mappedOutline;
	}

	public Vector2I GetDimensions() {
		var dimensions = simulation.AsGodotObject().Call("get_dimensions").AsVector2I();
		return dimensions;
	}

	public byte[] GetRenderData() {
		byte[] renderData = simulation.AsGodotObject().Call("get_render_data").AsByteArray();
		return renderData;
	}

	public bool IsInBounds(Vector2I position) {
		bool isInBounds = simulation.AsGodotObject().Call("is_in_bounds", position.X, position.Y).AsBool();
		return isInBounds;
	}

	public void Draw(Vector2I position) {
		simulation.AsGodotObject().Call("draw_particle", position.X, position.Y, 0);
	}

	public void Step(int iterations) {
		simulation.AsGodotObject().Call("step", iterations);
	}
}
