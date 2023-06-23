
== Point Lights
	- Lights with a position that emit light in all direction
	- Need to determine direction vector manually
	- Get difference between light position and fragment position
	- Apply directional lighting maths to the calculated direction vector

= Point Lights - Attenuation
	- Directional Lights simulate infinite distance, so distance does effect
		lighting power
	- Point Lights havae positions, distance from point being lit changes
		power of lighting
	- One possible solution: Linear drop-off
	- Have the lighting power drop off in direct proportion with the distance
		of the light source (simple, but not realistic)

	- In reality, light intensity drops qucky with distance
	- But the further you are, the slower it decreases 
	- (Like a series of 1/x_i)
	- For positive values, the reciprocal of a quadratic function can create
		this effect
	- ie. 1 / (ax^2 + bx + c)
	- Where x is the distance betwen the light source and fragment

	Attenuation factor = 1.0 /
		quadratic * distance^2 + linear * distance + constant

== Spot Lights
	- Work the same as Point Lights in theory
	- have position, attenuation
	- Also have a direction aand a cut off angle
	- Direction: where the spot light is facing
	- Cut-off angle: The angle describing the 'edges' of the light, from 
		the direction vector
	- We need a way to compare the 'angle of fragment' to the 'cut off angle'
	- Use the dot product again

	- To find cut off angle:

	angleToFragment = dot(lightVector, lightDirection)

	- lightVector: the vector from the light to the fragment
	- lightDirection: the direction the spot light is facing
	- So angleToFragment will be a value between 0 and 1,
		representing the angle between the two.
	- Can simply do cos(cutOffAngle) for the Cut Off Angle
	- Larger value: smaller angle
	- Smaller value: Larger angle

	- if angleToFragment > cos(cutOffAngle) then it is within the 'spotlight'
		and we should apply lighting, Otherwise don't apply light

= Spot Lights - Soft Edges
	- Current approach has sharp cut off edges at the spot
	- Creates unrealisitic spot light (although might look cool in some styles)
	- Need to soften the light on the border of the cut off ranges
	- Use the result of the pre-computed dot product as a factor
	- however the dot product won't scale very well
	- ie. if cut off is 10 degrees:
		- minimum dot product is cos(10deg) = 0.98
		- dot product range will be 0.98 - 1.00
		- resulting in an unnoticable fade
	- The solution is to scale the dot product to fit a range [0,1]

	- Formula to scale value between ranges:

	new value = (newRangeMax - newRangeMin)(originalValue - originalRangeMin) /
						originalRangeMax - originalRangeMin

	- newRangeMin is 0, newRangeMax is 1, so numerator is just:
		originalValue - originalRangeMin
	- originalRangeMax is 1
	- So with some inverting of min and max values:

	spotLightFade = 1 - ((1 - angleToFragment) / (1 - cutOffAngle));

	- After calculating, multiply by spotLightFade effect
	 
	color = spotLightColor * spotLightFade;