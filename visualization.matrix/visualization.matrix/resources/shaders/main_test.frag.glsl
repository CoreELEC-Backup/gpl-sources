void main()
{
  float y = ( gl_FragCoord.y / iResolution.y ) * 26.0;
  float x = 1.0 - ( gl_FragCoord.x / iResolution.x );
  float b = fract( pow( 2.0, floor(y) ) + x );
  if (fract(y) >= 0.9)
  b = 0.0;
  FragColor = vec4(b, b, b, 1.0 );
}
