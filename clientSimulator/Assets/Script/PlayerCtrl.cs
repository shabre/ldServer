using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerCtrl : MonoBehaviour {
        private Transform tr;
        public float speed = 8.0f;
	// Use this for initialization
	void Start () {
                tr= this.gameObject.GetComponent<Transform>();
	}
	
	// Update is called once per frame
	void Update () {
                float h = Input.GetAxis("Horizontal"); // -1.0f ~ 0.0f ~ +1.0f
                float v = Input.GetAxis("Vertical");
                float r = Input.GetAxis("Mouse X");

        //Debug.Log("v=" + v);

                Vector3 moveDir = (Vector3.forward * v) + (Vector3.right * h);
                tr.Translate(moveDir.normalized * Time.deltaTime * speed);
                tr.Rotate(Vector3.up * 80.0f * Time.deltaTime * r);
	}
}
