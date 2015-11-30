﻿using UnityEngine;
using System.Collections;
using UnityEditor;
using UnityEditor.Callbacks;
using System.IO;

/// <summary>
/// Ovrvision Custom Editor
/// </summary>
[CustomEditor( typeof(Ovrvision) )]
public class OvrvisionEditor : Editor {

	private const int OV_SET_AUTOMODE = (-1);

	public override void OnInspectorGUI() {
		Ovrvision obj = target as Ovrvision;
       
		EditorGUILayout.LabelField( "Ovrvision Status" );
		if (obj.CameraStatus())
			EditorGUILayout.HelpBox( "Opened", MessageType.Info, true );
		else
			EditorGUILayout.HelpBox( "Closed", MessageType.Error, true );

		EditorGUILayout.Space();

		obj.overlaySettings = EditorGUILayout.Toggle("Overlay Ovrvision settings", obj.overlaySettings);
		if (obj.overlaySettings)
		{
			obj.conf_exposure = EditorGUILayout.IntSlider("Exposure", obj.conf_exposure, 0, 32767);
			obj.conf_gain = EditorGUILayout.IntSlider("Gain", obj.conf_gain, 1, 47);
			obj.conf_blc = EditorGUILayout.IntSlider("BLC", obj.conf_blc, 0, 1023);

			obj.conf_wb_auto = EditorGUILayout.Toggle("WhiteBalance Auto", obj.conf_wb_auto);
			if (!obj.conf_wb_auto) {
				obj.conf_wb_r = EditorGUILayout.IntSlider("WhiteBalance R", obj.conf_wb_r, 0, 4095);
				obj.conf_wb_g = EditorGUILayout.IntSlider("WhiteBalance G", obj.conf_wb_g, 0, 4095);
				obj.conf_wb_b = EditorGUILayout.IntSlider("WhiteBalance B", obj.conf_wb_b, 0, 4095);
			}
			if (GUILayout.Button("Default config", GUILayout.Width(200)))
			{
				obj.conf_exposure = 12960;
				obj.conf_gain = 8;
				obj.conf_blc = 32;
				obj.conf_wb_r = 1474;
				obj.conf_wb_g = 1024;
				obj.conf_wb_b = 1738;
				obj.conf_wb_auto = true;
			}
		}

		EditorGUILayout.Space();

		//Ovrvision AR
		obj.useOvrvisionAR = EditorGUILayout.Toggle("Use the OvrvisionAR", obj.useOvrvisionAR);
		obj.ARsize = EditorGUILayout.FloatField("AR Marker size(meter)", obj.ARsize);

		EditorGUILayout.Space();

		string[] planeshader = { "Normal Shader", "Chroma-key Shader", "Hand Mask Shader" };
		obj.camViewShader = EditorGUILayout.Popup("Camera Overlay", obj.camViewShader, planeshader);

		if (obj.camViewShader == 1)
		{	//Chroma-Key
			obj.chroma_hue.x = EditorGUILayout.Slider("Max Hue", obj.chroma_hue.x, 0.0f, 1.0f);
			obj.chroma_hue.y = EditorGUILayout.Slider("Min Hue", obj.chroma_hue.y, 0.0f, 1.0f);
			obj.chroma_saturation.x = EditorGUILayout.Slider("Max Saturation", obj.chroma_saturation.x, 0.0f, 1.0f);
			obj.chroma_saturation.y = EditorGUILayout.Slider("Min Saturation", obj.chroma_saturation.y, 0.0f, 1.0f);
			obj.chroma_brightness.x = EditorGUILayout.Slider("Max Brightness", obj.chroma_brightness.x, 0.0f, 1.0f);
			obj.chroma_brightness.y = EditorGUILayout.Slider("Min Brightness ", obj.chroma_brightness.y, 0.0f, 1.0f);
		} else if (obj.camViewShader == 2)
		{	//Hand Mask

		}

		//changed param
		if (GUI.changed) {
			obj.UpdateOvrvisionSetting();
		}
		EditorUtility.SetDirty( target );	//editor set
	}
}