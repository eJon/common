<?php
namespace Home\Controller;
use Think\Controller;
class IndexController extends Controller {
    public function index(){
    	//首页显示热门职位
    	$hot_job_list = $this->get_hot_jobs();
    	/*array(array("title"=>"java R & D",
    		"logo"=>"./Uploads/Picture/2015-01-01/baidu.jpg",
    		"redirect"=> "#",
    		'company'=>'百度科技有限公司',
    		'salary'=>'年薪：20-30万',
    		'location'=>'北京',
    		'pub_tm'=>'10分钟前',
    		'label'=>array('0'=>'发展空间大','1'=>'五险一金','2'=>'扁平管理'))
    	);*/
    	$this->assign('hot_job_list',$hot_job_list);
    	$this->assign('title','翘课网，让找工作变得简单');
    	$this->assign('keywords','翘课网,找工作,在线教育,招聘,求职');
    	$this->assign('description','翘课网，让找工作变得简单');
    	$this->assign('head_title','热门职位');
        $this->display();
    }
    private function get_hot_jobs() {
    	$hot_job_list = M('job_detail')->limit(10)->select();
    	foreach($hot_job_list as $key=>$value) {
    		
    		$hot_job_list[$key]['label'] = explode('|', $value['label']);
    		$hot_job_list[$key]['redirect'] = __ROOT__."/index.php?c=page&a=index&id=".$value['id'];
    	}
    	
        return $hot_job_list;
    }
    /**
    */
    public function search() {

    }
}