using Godot;
using System;
using System.Threading.Tasks;

public partial class SplashManager : Control
{
    private ColorRect _ball;
    private ColorRect _loadingBar;
    private Label _percentLabel;
    private int[] _milestones = { 1, 3, 8, 15, 35, 46, 58, 71, 89, 94, 99 };

    public override void _Ready()
    {
        // Link the nodes
        _ball = GetNode<ColorRect>("Ball");
        _loadingBar = GetNode<ColorRect>("LoadingBar");
        
        // Create a label for the percentage numbers
        _percentLabel = new Label();
        _percentLabel.Name = "PercentLabel";
        _percentLabel.Position = new Vector2(_loadingBar.Position.X + 310, _loadingBar.Position.Y - 5);
        AddChild(_percentLabel);

        // Apply the Procedural Football Shader
        var mat = new ShaderMaterial();
        var shader = new Shader();
        shader.Code = @"
            shader_type canvas_item;
            void fragment() {
                vec2 center = vec2(0.5, 0.5);
                float dist = distance(UV, center);
                if (dist > 0.5) { discard; }
                float pattern = step(0.45, sin(UV.x * 15.0) * cos(UV.y * 15.0));
                COLOR = mix(vec4(1.0), vec4(0.1, 0.1, 0.1, 1.0), pattern);
            }
        ";
        mat.Shader = shader;
        _ball.Material = mat;

        StartLoadingSequence();
    }

    public override void _Process(double delta)
    {
        // Spinning the ball
        _ball.Rotation += 8.0f * (float)delta;
    }

    private async void StartLoadingSequence()
    {
        foreach (int val in _milestones)
        {
            // Update UI
            float progressWidth = (val / 100.0f) * 300.0f; 
            _loadingBar.Size = new Vector2(progressWidth, _loadingBar.Size.Y);
            _percentLabel.Text = val.ToString() + "%";
            
            // Wait to total ~3 seconds
            await ToSignal(GetTree().CreateTimer(0.25f), "timeout");
        }

        FadeToHome();
    }

    private void FadeToHome()
    {
        var tween = CreateTween();
        tween.TweenProperty(this, "modulate:a", 0.0f, 1.0f);
        tween.Finished += () => {
            _percentLabel.Modulate = new Color(1, 1, 1, 1);
            _percentLabel.Text = "HomePage Coming Soon";
            _percentLabel.HorizontalAlignment = HorizontalAlignment.Center;
            _percentLabel.Position = new Vector2(GetViewportRect().Size.X / 2 - 100, GetViewportRect().Size.Y / 2);
        };
    }
}
