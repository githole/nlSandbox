struct GSIn
{
	float4 P : POS;
	float4 N : NORMAL;
	float4 C  : TEXCOORD0;
	float4 OP : TEXCOORD1;
};

struct PSIn
{
	float4 P : SV_Position;
	float4 N : TEXCOORD0;
	float4 C : TEXCOORD1;
};

cbuffer Const2 : register(b2)
{ f4x4 vp; }



void Rotate(f3 rotang, out float4x4 mat )
{
	f2 roty;
	f2 rotz;
	sincos( rotang.y, roty.x, roty.y );
	sincos( rotang.z, rotz.x, rotz.y );
	mat._11=  rotz.y * roty.y;
	mat._12=  rotz.x * roty.y;
	mat._13=          -roty.x;
	mat._14= 0;
	mat._21= -rotz.x;
	mat._22=  rotz.y;
	mat._23= 0.0f;
	mat._24= 0;
	mat._31=  roty.x;
	mat._32= 0.0f;
	mat._33=  roty.y;
	mat._34= 0;
	mat._41= 0;
	mat._42= 0;
	mat._43= 0;
	mat._44= 1;
}

[maxvertexcount(6)]
void main( triangle GSIn input[3], inout TriangleStream<PSIn> OutputStream )
{	
	PSIn output = (PSIn)0;
	const f4 col[] = {f4(1,1,1,1), f4(0.8, 0.1, 0.1,0.7*min(1,(sceneTime.x-0.1)*10.0))};
	f3 op  = input[0].OP.xyz;
	f3 ct  = (input[0].P + input[1].P + input[2].P).xyz * 0.333333;
	f3 dir = normalize(input[0].N + input[1].N + input[2].N) + f3(0,1,0);
	float4x4 rotmat;
	f3 rrr = f3(rand(op.x)*30.0,rand(op.z)*30.0, rand(op.y)*30.0);
	
	f3 st = max(0,sceneTime.x - 0.2);
	Rotate(rrr * st, rotmat);
	f3 offset = f3(dir.x,dir.y * abs(op.z)*sin(op.z/2),dir.z) * st * st * 3;
	
	if (input[0].C.x > 0)
	{
		for( uint i=0; i<3; i++ )
		{
			output.P = input[i].P;
			output.P.xyz -= ct;
			output.P = mul(rotmat, output.P);
			output.P.xyz += ct + offset;
			output.P = mul(output.P, vp);
			// output.P = input[i].P + f4(dir * sceneTime.x,0);
			output.N = input[i].N;
			float sy = 2*cos((ct.y+20)/20.0);
			output.C = col[1] * (max(0,dot(mul(rotmat,dir),f3(0,-1,0)))*0.5+ 0.5);
			output.C *= f4(sy,sy,sy*1.5,1);
			output.C += (f4(.05,.05,.05,0) * sin(ct.y*sceneTime.x));
			OutputStream.Append( output );
		}
		OutputStream.RestartStrip();
	}
	else
	{
	/*
		for( uint i=0; i<3; i++ )
		{
			output.P = input[i].P;
			output.N = input[i].N;
			output.C = col[0];
			OutputStream.Append( output );
		}
		OutputStream.RestartStrip();
		*/
	}
}
